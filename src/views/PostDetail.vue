<template>
  <div>
    <router-link to="/posts" class="inline-flex items-center text-gray-500 hover:text-blue-600 transition-colors mb-6 group">
      <svg class="w-4 h-4 mr-1 group-hover:-translate-x-0.5 transition-transform" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M15 19l-7-7 7-7"/></svg>
      返回文章列表
    </router-link>

    <div v-if="loading" class="text-center py-20 text-gray-400">
      <svg class="animate-spin w-8 h-8 mx-auto mb-3" fill="none" viewBox="0 0 24 24"><circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"/><path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4z"/></svg>
      加载中...
    </div>
    <div v-else-if="error" class="text-center py-20">
      <p class="text-red-500 mb-2">{{ error }}</p>
      <router-link to="/posts" class="text-blue-600 hover:underline text-sm">返回文章列表</router-link>
    </div>
    <template v-else>
      <!-- Article header -->
      <div class="mb-8">
        <div class="flex items-center gap-3 text-sm text-gray-500 mb-3">
          <div class="w-8 h-8 rounded-full bg-gradient-to-br from-blue-500 to-purple-600 flex items-center justify-center text-white text-xs font-bold">
            {{ (profile.name || '作').charAt(0).toUpperCase() }}
          </div>
          <span>{{ profile.name }}</span>
          <span class="text-gray-300">·</span>
          <span>{{ post.date }}</span>
          <span v-if="post.category" class="ml-auto category-badge">{{ post.category }}</span>
        </div>
        <h1 class="text-3xl sm:text-4xl font-bold text-gray-900 leading-tight">{{ post.title }}</h1>
        <div v-if="post.tags && post.tags.length" class="flex flex-wrap gap-2 mt-4">
          <span v-for="tag in post.tags" :key="tag" class="tag-pill">{{ tag }}</span>
        </div>
      </div>

      <!-- Article body -->
      <article class="bg-white rounded-2xl shadow-sm border border-gray-100 p-6 sm:p-10">
        <div class="prose max-w-none" v-html="post.content" />
      </article>

      <!-- Utterances Comments -->
      <div class="mt-8 bg-white rounded-2xl shadow-sm border border-gray-100 p-6 sm:p-10">
        <h3 class="text-lg font-semibold text-gray-900 mb-2">评论</h3>
        <p v-if="!utterancesLoaded" class="text-sm text-gray-400 mb-4">
          加载评论需要 GitHub 登录。
          <span v-if="!repoSet" class="text-orange-500">请设置环境变量 <code class="bg-gray-100 px-1 rounded text-xs">VITE_UTTERANCES_REPO</code></span>
        </p>
        <div id="utterances-container" />
      </div>
    </template>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, nextTick } from 'vue'
import { useRoute } from 'vue-router'
import { marked } from 'marked'

const route = useRoute()
const post = ref({})
const loading = ref(true)
const error = ref('')
const profile = ref({ name: '作者' })
const utterancesLoaded = ref(false)

const repoSet = computed(() => !!import.meta.env.VITE_UTTERANCES_REPO)

onMounted(async () => {
  try {
    const [postRes, profileRes] = await Promise.all([
      fetch(`/api/posts/${route.params.slug}`),
      fetch('/api/profile'),
    ])
    if (!postRes.ok) { error.value = '文章不存在'; return }
    const data = await postRes.json()
    post.value = {
      ...data,
      content: marked(data.content || ''),
    }
    if (profileRes.ok) profile.value = await profileRes.json()
    loadUtterances()
  } catch (e) {
    error.value = e.message
  } finally {
    loading.value = false
  }
})

function loadUtterances() {
  const repo = import.meta.env.VITE_UTTERANCES_REPO
  if (!repo) return
  nextTick(() => {
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
  })
}
</script>

<style scoped>
.category-badge {
  font-size: 0.75rem;
  padding: 0.125rem 0.5rem;
  border-radius: 9999px;
  background: #f3f4f6;
  color: #6b7280;
}
.tag-pill {
  font-size: 0.75rem;
  padding: 0.25rem 0.625rem;
  border-radius: 9999px;
  background: #eff6ff;
  color: #2563eb;
}
</style>
