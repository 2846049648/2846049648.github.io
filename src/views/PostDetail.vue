<template>
  <div>
    <router-link to="/posts" class="text-blue-600 hover:underline mb-4 inline-block">&larr; 返回文章列表</router-link>
    <div v-if="loading" class="text-center py-12 text-gray-500">加载中...</div>
    <div v-else-if="error" class="text-center py-12 text-red-500">{{ error }}</div>
    <article v-else class="bg-white rounded-lg shadow">
      <!-- Header -->
      <div class="p-8 pb-0">
        <h1 class="text-3xl font-bold mb-4">{{ post.title }}</h1>
        <div class="flex items-center space-x-4 mb-6">
          <div class="w-10 h-10 rounded-full bg-blue-100 flex items-center justify-center text-blue-600 font-bold">
            {{ (profile.name || '作').charAt(0) }}
          </div>
          <div>
            <p class="text-sm font-medium text-gray-900">{{ profile.name }}</p>
            <p class="text-sm text-gray-500">{{ post.date }}</p>
          </div>
          <div class="flex-1" />
          <div class="flex items-center space-x-2">
            <span v-if="post.category" class="bg-gray-100 text-gray-600 px-2 py-0.5 rounded text-sm">{{ post.category }}</span>
            <span v-for="tag in post.tags" :key="tag" class="bg-blue-100 text-blue-600 px-2 py-0.5 rounded text-sm">{{ tag }}</span>
          </div>
        </div>
      </div>
      <!-- Content -->
      <div class="px-8 pb-8 prose max-w-none" v-html="post.content" />
    </article>

    <!-- Giscus Comments -->
    <div class="mt-8 bg-white rounded-lg shadow p-8">
      <h3 class="text-lg font-semibold mb-4">评论</h3>
      <div v-if="!giscusReady" class="text-gray-500 text-sm">
        评论系统需配置 Giscus。请在 <code class="bg-gray-100 px-1 rounded">src/components/Giscus.vue</code> 中设置你的 repo 信息。
      </div>
      <component v-else :is="'div'" ref="giscusRef" />
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, nextTick } from 'vue'
import { useRoute } from 'vue-router'
import { marked } from 'marked'

const route = useRoute()
const post = ref({})
const loading = ref(true)
const error = ref('')
const profile = ref({ name: '作者' })
const giscusReady = ref(false)

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

    // Load Giscus
    await loadGiscus()
  } catch (e) {
    error.value = e.message
  } finally {
    loading.value = false
  }
})

async function loadGiscus() {
  const repo = import.meta.env.VITE_GISCUS_REPO
  if (!repo) return

  await nextTick()
  const script = document.createElement('script')
  script.src = 'https://giscus.app/client.js'
  script.setAttribute('data-repo', repo)
  script.setAttribute('data-repo-id', import.meta.env.VITE_GISCUS_REPO_ID || '')
  script.setAttribute('data-category', import.meta.env.VITE_GISCUS_CATEGORY || 'Announcements')
  script.setAttribute('data-category-id', import.meta.env.VITE_GISCUS_CATEGORY_ID || '')
  script.setAttribute('data-mapping', 'pathname')
  script.setAttribute('data-strict', '0')
  script.setAttribute('data-reactions-enabled', '1')
  script.setAttribute('data-emit-metadata', '0')
  script.setAttribute('data-input-position', 'bottom')
  script.setAttribute('data-theme', 'light')
  script.setAttribute('data-lang', 'zh-CN')
  script.setAttribute('crossorigin', 'anonymous')
  script.async = true
  document.body.appendChild(script)
  giscusReady.value = true
}
</script>
