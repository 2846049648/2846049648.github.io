<template>
  <div>
    <!-- Hero -->
    <section class="relative overflow-hidden rounded-2xl bg-gradient-to-br from-blue-600 via-indigo-600 to-purple-700 text-white p-8 sm:p-16 mb-12">
      <div class="absolute inset-0 bg-[url('data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iNjAiIGhlaWdodD0iNjAiIHZpZXdCb3g9IjAgMCA2MCA2MCIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48ZyBmaWxsPSJub25lIiBmaWxsLXJ1bGU9ImV2ZW5vZGQiPjxnIGZpbGw9IiNmZmYiIGZpbGwtb3BhY2l0eT0iMC4wNSI+PGNpcmNsZSBjeD0iMzAiIGN5PSIzMCIgcj0iMiIvPjwvZz48L2c+PC9zdmc+')] opacity-50" />
      <div class="relative z-10">
        <div class="flex items-center space-x-4 mb-6">
          <div class="w-16 h-16 sm:w-20 sm:h-20 rounded-full bg-white/20 flex items-center justify-center text-3xl sm:text-4xl font-bold ring-2 ring-white/40 overflow-hidden">
            <img v-if="profile.avatar" :src="profile.avatar" class="w-full h-full object-cover" />
            <span v-else>{{ (profile.name || '我').charAt(0) }}</span>
          </div>
          <div>
            <h1 class="text-3xl sm:text-4xl font-bold">{{ greeting }}</h1>
            <p class="mt-2 text-blue-100 text-lg">{{ profile.bio }}</p>
          </div>
        </div>
        <div class="flex flex-wrap gap-3 mt-8">
          <router-link to="/posts" class="inline-flex items-center px-6 py-2.5 bg-white text-blue-700 rounded-full font-medium hover:bg-blue-50 transition-colors shadow-lg shadow-black/10">
            浏览文章
            <svg class="w-4 h-4 ml-2" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M17 8l4 4m0 0l-4 4m4-4H3"/></svg>
          </router-link>
          <router-link to="/gallery" class="inline-flex items-center px-6 py-2.5 bg-white/20 text-white rounded-full font-medium hover:bg-white/30 transition-colors backdrop-blur-sm">
            查看相册
            <svg class="w-4 h-4 ml-2" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 16l4.586-4.586a2 2 0 012.828 0L16 16m-2-2l1.586-1.586a2 2 0 012.828 0L20 14m-6-6h.01M6 20h12a2 2 0 002-2V6a2 2 0 00-2-2H6a2 2 0 00-2 2v12a2 2 0 002 2z"/></svg>
          </router-link>
        </div>
      </div>
    </section>

    <!-- Quick links -->
    <div class="grid grid-cols-1 sm:grid-cols-3 gap-4 mb-12">
      <router-link to="/posts" class="group bg-white rounded-xl p-6 shadow-sm hover:shadow-md transition-all border border-gray-100">
        <div class="w-10 h-10 rounded-lg bg-blue-100 text-blue-600 flex items-center justify-center mb-3 group-hover:scale-110 transition-transform">
          <svg class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M19 20H5a2 2 0 01-2-2V6a2 2 0 012-2h10a2 2 0 012 2v1m2 13a2 2 0 01-2-2V7m2 13a2 2 0 002-2V9a2 2 0 00-2-2h-2m-4-3H9M7 16h6M7 8h6v4H7V8z"/></svg>
        </div>
        <h3 class="font-semibold text-gray-900 mb-1">文章</h3>
        <p class="text-sm text-gray-500">阅读最新技术分享与随笔</p>
      </router-link>
      <router-link to="/gallery" class="group bg-white rounded-xl p-6 shadow-sm hover:shadow-md transition-all border border-gray-100">
        <div class="w-10 h-10 rounded-lg bg-purple-100 text-purple-600 flex items-center justify-center mb-3 group-hover:scale-110 transition-transform">
          <svg class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 16l4.586-4.586a2 2 0 012.828 0L16 16m-2-2l1.586-1.586a2 2 0 012.828 0L20 14m-6-6h.01M6 20h12a2 2 0 002-2V6a2 2 0 00-2-2H6a2 2 0 00-2 2v12a2 2 0 002 2z"/></svg>
        </div>
        <h3 class="font-semibold text-gray-900 mb-1">相册</h3>
        <p class="text-sm text-gray-500">浏览生活记录与精彩瞬间</p>
      </router-link>
      <router-link to="/downloads" class="group bg-white rounded-xl p-6 shadow-sm hover:shadow-md transition-all border border-gray-100">
        <div class="w-10 h-10 rounded-lg bg-green-100 text-green-600 flex items-center justify-center mb-3 group-hover:scale-110 transition-transform">
          <svg class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 10v6m0 0l-3-3m3 3l3-3m2 8H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z"/></svg>
        </div>
        <h3 class="font-semibold text-gray-900 mb-1">下载</h3>
        <p class="text-sm text-gray-500">获取实用资源与文件</p>
      </router-link>
    </div>

    <!-- Recent posts -->
    <section v-if="recentPosts.length">
      <h2 class="text-xl font-bold text-gray-900 mb-4">最近更新</h2>
      <div class="space-y-3">
        <router-link
          v-for="post in recentPosts"
          :key="post.slug"
          :to="`/posts/${post.slug}`"
          class="block bg-white rounded-xl p-5 shadow-sm hover:shadow-md transition-all border border-gray-100"
        >
          <div class="flex items-center justify-between">
            <h3 class="font-medium text-gray-900">{{ post.title }}</h3>
            <span class="text-sm text-gray-400">{{ post.date }}</span>
          </div>
          <p v-if="post.excerpt" class="text-sm text-gray-500 mt-1 line-clamp-1">{{ post.excerpt }}</p>
        </router-link>
      </div>
    </section>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'

const profile = ref({ name: '我', bio: '' })
const recentPosts = ref([])

const greeting = computed(() => {
  const h = new Date().getHours()
  if (h < 6) return '夜深了，还不睡吗？'
  if (h < 12) return '早上好！☀️'
  if (h < 14) return '中午好！'
  if (h < 18) return '下午好！'
  return '晚上好！🌙'
})

onMounted(async () => {
  try {
    const profileUrl = import.meta.env.DEV ? '/api/profile' : '/profile.json'
    const postsUrl = import.meta.env.DEV ? '/api/posts' : '/posts/index.json'
    const [profileRes, postsRes] = await Promise.all([
      fetch(profileUrl),
      fetch(postsUrl),
    ])
    if (profileRes.ok) profile.value = await profileRes.json()
    if (postsRes.ok) {
      const all = await postsRes.json()
      recentPosts.value = all.slice(0, 5)
    }
  } catch {}
})
</script>
