<template>
  <div>
    <router-link to="/posts" class="inline-flex items-center transition-colors mb-6 group" :style="{ color: 'var(--text-muted)' }" @click.prevent="goBack">
      <svg class="w-4 h-4 mr-1 group-hover:-translate-x-0.5 transition-transform" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M15 19l-7-7 7-7"/></svg>
      返回文章列表
    </router-link>

    <div v-if="loading" class="text-center py-20" :style="{ color: 'var(--text-light)' }">
      <svg class="animate-spin w-8 h-8 mx-auto mb-3" fill="none" viewBox="0 0 24 24"><circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"/><path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4z"/></svg>
      加载中...
    </div>
    <div v-else-if="error" class="text-center py-20">
      <p class="text-red-500 mb-2">{{ error }}</p>
      <router-link to="/posts" class="text-blue-600 hover:underline text-sm">返回文章列表</router-link>
    </div>
    <template v-else>
      <div class="flex gap-8">
        <!-- TOC Sidebar (desktop, left) -->
        <div v-if="headings.length > 1" class="hidden lg:block w-56 flex-shrink-0">
          <div class="sticky top-24">
            <h4 class="text-xs font-semibold uppercase tracking-wider mb-3" :style="{ color: 'var(--text-light)' }">目录</h4>
            <nav>
              <a
                v-for="h in headings" :key="h.id"
                :href="'#' + h.id"
                :class="['toc-link', h.level, { active: activeId === h.id }]"
                @click.prevent="scrollToHeading(h.id)"
              >{{ h.text }}</a>
            </nav>
          </div>
        </div>

        <!-- Article -->
        <div class="flex-1 min-w-0">
          <!-- Article header -->
          <div class="mb-8">
            <div class="flex items-center gap-3 text-sm mb-3" :style="{ color: 'var(--text-muted)' }">
              <div class="w-8 h-8 rounded-full bg-gradient-to-br from-blue-500 to-purple-600 flex items-center justify-center text-white text-xs font-bold">
                {{ (profile.name || '作').charAt(0).toUpperCase() }}
              </div>
              <span>{{ profile.name }}</span>
              <span class="text-gray-300">·</span>
              <span>{{ post.date }}</span>
              <span v-if="post.category" class="ml-auto category-badge">{{ post.category }}</span>
            </div>
            <h1 class="text-3xl sm:text-4xl font-bold leading-tight" :style="{ color: 'var(--text-primary)' }">{{ post.title }}</h1>
            <div v-if="post.tags && post.tags.length" class="flex flex-wrap gap-2 mt-4">
              <router-link
                v-for="tag in post.tags" :key="tag"
                :to="`/posts?tag=${encodeURIComponent(tag)}`"
                class="tag-pill"
              >{{ tag }}</router-link>
            </div>
          </div>

          <!-- Article body -->
          <article class="rounded-2xl shadow-sm border p-6 sm:p-10" :style="{ background: 'var(--bg-surface)', borderColor: 'var(--border-color)' }">
            <div class="prose max-w-none" v-html="renderedContent" ref="articleRef" />
          </article>

          <!-- Prev / Next -->
          <div v-if="prevPost || nextPost" class="mt-8 flex gap-4 flex-col sm:flex-row">
            <router-link
              v-if="prevPost"
              :to="`/posts/${prevPost.slug}`"
              class="flex-1 rounded-xl border p-4 transition-all hover:shadow-md"
              :style="{ background: 'var(--bg-surface)', borderColor: 'var(--border-color)' }"
            >
              <div class="text-xs mb-1" :style="{ color: 'var(--text-light)' }">← 上一篇</div>
              <div class="font-medium truncate" :style="{ color: 'var(--text-primary)' }">{{ prevPost.title }}</div>
            </router-link>
            <div v-else class="flex-1" />
            <router-link
              v-if="nextPost"
              :to="`/posts/${nextPost.slug}`"
              class="flex-1 rounded-xl border p-4 text-right transition-all hover:shadow-md"
              :style="{ background: 'var(--bg-surface)', borderColor: 'var(--border-color)' }"
            >
              <div class="text-xs mb-1" :style="{ color: 'var(--text-light)' }">下一篇 →</div>
              <div class="font-medium truncate" :style="{ color: 'var(--text-primary)' }">{{ nextPost.title }}</div>
            </router-link>
            <div v-else class="flex-1" />
          </div>

          <!-- Comments -->
          <div class="mt-8 rounded-2xl shadow-sm border p-6 sm:p-10" :style="{ background: 'var(--bg-surface)', borderColor: 'var(--border-color)' }">
            <h3 class="text-lg font-semibold mb-4" :style="{ color: 'var(--text-primary)' }">评论</h3>
            <div v-if="!repoSet" class="text-sm" :style="{ color: 'var(--text-light)' }">
              评论功能未启用（需配置 <code class="bg-gray-100 dark:bg-gray-800 px-1 rounded text-xs">VITE_UTTERANCES_REPO</code>）
            </div>
            <div v-else-if="!utterancesLoaded" class="text-sm" :style="{ color: 'var(--text-light)' }">
              评论加载中...
            </div>
            <div id="utterances-container" />
          </div>
        </div>

      </div>
    </template>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, nextTick } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { marked } from 'marked'
import { markedHighlight } from 'marked-highlight'
import hljs from 'highlight.js'
import { useToc } from '../composables/useToc.js'

const route = useRoute()
const router = useRouter()
const post = ref({})
const loading = ref(true)
const error = ref('')
const profile = ref({ name: '作者' })
const utterancesLoaded = ref(false)
const allPosts = ref([])
const articleRef = ref(null)

const { headings, activeId, generateToc, initScrollSpy } = useToc()

const repoSet = computed(() => !!import.meta.env.VITE_UTTERANCES_REPO)

const renderedContent = ref('')

// Configure marked with highlight.js
marked.use(markedHighlight({
  langPrefix: 'hljs language-',
  highlight(code, lang) {
    if (lang && hljs.getLanguage(lang)) {
      try { return hljs.highlight(code, { language: lang }).value } catch {}
    }
    return hljs.highlightAuto(code).value
  },
}))

function renderMarkdown(text) {
  const rawHtml = marked.parse(text)
  return generateToc(rawHtml)
}

// Prev/Next
const prevPost = computed(() => {
  const idx = allPosts.value.findIndex(p => p.slug === route.params.slug)
  return idx < allPosts.value.length - 1 ? allPosts.value[idx + 1] : null
})
const nextPost = computed(() => {
  const idx = allPosts.value.findIndex(p => p.slug === route.params.slug)
  return idx > 0 ? allPosts.value[idx - 1] : null
})

onMounted(async () => {
  try {
    const slug = route.params.slug
    const [postsRes] = await Promise.all([
      fetch(import.meta.env.DEV ? '/api/posts' : '/posts/index.json'),
    ])
    if (postsRes.ok) {
      allPosts.value = await postsRes.json()
    }

    if (import.meta.env.DEV) {
      const [postRes, profileRes] = await Promise.all([
        fetch(`/api/posts/${slug}`),
        fetch('/api/profile'),
      ])
      if (!postRes.ok) { error.value = '文章不存在'; return }
      const data = await postRes.json()
      renderedContent.value = renderMarkdown(data.content || '')
      post.value = { ...data }
      if (profileRes.ok) profile.value = await profileRes.json()
    } else {
      const [mdRes, profileRes] = await Promise.all([
        fetch(`/posts/${slug}.md`),
        fetch('/profile.json'),
      ])
      if (!mdRes.ok) { error.value = '文章不存在'; return }
      const text = await mdRes.text()
      const { data, content } = parseFrontMatter(text)
      renderedContent.value = renderMarkdown(content)
      post.value = { ...data, slug }
      if (profileRes.ok) profile.value = await profileRes.json()
    }
  } catch (e) {
    error.value = e.message
  } finally {
    loading.value = false
    await nextTick()
    initScrollSpy()
    addCopyButtons()
    if (import.meta.env.VITE_UTTERANCES_REPO) {
      nextTick(loadUtterances)
    }
  }
})

function scrollToHeading(id) {
  const el = document.getElementById(id)
  if (el) {
    el.scrollIntoView({ behavior: 'smooth', block: 'start' })
    history.replaceState(null, '', '#' + id)
    activeId.value = id
  }
}

function addCopyButtons() {
  if (!articleRef.value) return
  articleRef.value.querySelectorAll('pre').forEach(pre => {
    if (pre.querySelector('.code-copy-btn')) return
    const btn = document.createElement('button')
    btn.className = 'code-copy-btn'
    btn.textContent = '复制'
    btn.addEventListener('click', async () => {
      const code = pre.querySelector('code')
      if (!code) return
      try {
        await navigator.clipboard.writeText(code.textContent)
        btn.textContent = '已复制!'
        btn.classList.add('copied')
        setTimeout(() => {
          btn.textContent = '复制'
          btn.classList.remove('copied')
        }, 2000)
      } catch {
        btn.textContent = '复制失败'
      }
    })
    pre.appendChild(btn)
  })
}

function goBack() {
  if (window.history.length > 1) {
    router.back()
  } else {
    router.push('/posts')
  }
}

function parseFrontMatter(text) {
  const match = text.match(/^---\n([\s\S]*?)\n---\n([\s\S]*)$/)
  if (!match) return { data: { title: '无标题', tags: [] }, content: text }
  const data = {}
  match[1].split('\n').forEach(line => {
    const idx = line.indexOf(':')
    if (idx === -1) return
    const key = line.slice(0, idx).trim()
    const val = line.slice(idx + 1).trim()
    if (val.startsWith('[') && val.endsWith(']')) {
      data[key] = val.slice(1, -1).split(',').map(s => s.trim().replace(/['"]/g, ''))
    } else {
      data[key] = val.replace(/^['"]|['"]$/g, '')
    }
  })
  return { data, content: match[2] }
}

function loadUtterances() {
  const repo = import.meta.env.VITE_UTTERANCES_REPO
  const container = document.getElementById('utterances-container')
  if (!container || container.querySelector('script')) return
  const script = document.createElement('script')
  script.src = 'https://utteranc.es/client.js'
  script.setAttribute('repo', repo)
  script.setAttribute('issue-term', 'pathname')
  script.setAttribute('label', 'comments')
  script.setAttribute('theme', 'github-light')
  script.setAttribute('crossorigin', 'anonymous')
  script.async = true
  container.appendChild(script)
  utterancesLoaded.value = true
}
</script>

<style scoped>
.category-badge {
  font-size: 0.75rem;
  padding: 0.125rem 0.5rem;
  border-radius: 9999px;
  background: var(--bg-muted);
  color: var(--text-muted);
}
.tag-pill {
  font-size: 0.75rem;
  padding: 0.25rem 0.625rem;
  border-radius: 9999px;
  background: #eff6ff;
  color: #2563eb;
  text-decoration: none;
  transition: all 0.2s;
}
.tag-pill:hover {
  background: #dbeafe;
}
.dark .tag-pill {
  background: rgba(37, 99, 235, 0.15);
  color: #60a5fa;
}
.dark .tag-pill:hover {
  background: rgba(37, 99, 235, 0.25);
}
</style>
