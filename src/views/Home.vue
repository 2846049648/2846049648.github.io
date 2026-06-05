<template>
  <div>
    <!-- Hero -->
    <section class="relative overflow-hidden rounded-2xl mb-12 min-h-[420px] flex items-center"
      style="background: linear-gradient(135deg, #0a1628 0%, #0f1929 50%, #0d1a2e 100%);">
      <!-- Animated grid overlay -->
      <div class="absolute inset-0 opacity-20"
        style="background-image:
          linear-gradient(rgba(0, 229, 255, 0.1) 1px, transparent 1px),
          linear-gradient(90deg, rgba(0, 229, 255, 0.1) 1px, transparent 1px);
          background-size: 40px 40px;" />
      <!-- Glow orbs -->
      <div class="absolute -top-32 -right-32 w-96 h-96 rounded-full opacity-10 blur-3xl"
        style="background: radial-gradient(circle, rgba(0, 229, 255, 0.4), transparent)" />
      <div class="absolute -bottom-32 -left-32 w-96 h-96 rounded-full opacity-10 blur-3xl"
        style="background: radial-gradient(circle, rgba(0, 255, 65, 0.3), transparent)" />

      <!-- Content -->
      <div class="relative z-10 w-full px-8 sm:px-16 py-12 sm:py-16">
        <div class="flex flex-col sm:flex-row items-center sm:items-start sm:space-x-6">
          <!-- Avatar -->
          <div class="w-20 h-20 sm:w-24 sm:h-24 rounded-full flex-shrink-0 flex items-center justify-center text-3xl sm:text-4xl font-bold overflow-hidden mb-4 sm:mb-0"
            style="border: 2px solid rgba(0, 229, 255, 0.3); box-shadow: 0 0 30px rgba(0, 229, 255, 0.15); background: rgba(0, 229, 255, 0.05); backdrop-filter: blur(8px);">
            <img v-if="profile.avatar" :src="profile.avatar" class="w-full h-full object-cover" />
            <span v-else class="gradient-text">{{ (profile.name || '我').charAt(0) }}</span>
          </div>
          <div class="text-center sm:text-left">
            <h1 class="text-3xl sm:text-5xl font-bold text-white mb-2">{{ greeting }}</h1>
            <p class="text-lg" style="color: #6b8cae;">{{ profile.bio }}</p>
            <div class="flex flex-wrap justify-center sm:justify-start gap-3 mt-6">
              <router-link to="/posts" class="btn-cyber btn-cyber-primary">
                浏览文章
                <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M17 8l4 4m0 0l-4 4m4-4H3"/></svg>
              </router-link>
              <router-link to="/gallery" class="btn-cyber btn-cyber-glass">
                查看相册
                <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 16l4.586-4.586a2 2 0 012.828 0L16 16m-2-2l1.586-1.586a2 2 0 012.828 0L20 14m-6-6h.01M6 20h12a2 2 0 002-2V6a2 2 0 00-2-2H6a2 2 0 00-2 2v12a2 2 0 002 2z"/></svg>
              </router-link>
            </div>
          </div>
        </div>
      </div>
    </section>

    <!-- Quick links -->
    <div class="grid grid-cols-1 sm:grid-cols-3 gap-4 mb-12">
      <router-link to="/posts" class="card-cyber p-6 group">
        <div class="w-12 h-12 rounded-xl flex items-center justify-center mb-4 group-hover:scale-110 transition-transform"
          style="background: linear-gradient(135deg, rgba(0,229,255,0.1), rgba(0,229,255,0.03)); color: var(--color-primary);">
          <svg class="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M19 20H5a2 2 0 01-2-2V6a2 2 0 012-2h10a2 2 0 012 2v1m2 13a2 2 0 01-2-2V7m2 13a2 2 0 002-2V9a2 2 0 00-2-2h-2m-4-3H9M7 16h6M7 8h6v4H7V8z"/></svg>
        </div>
        <h3 class="text-lg font-semibold mb-1" :style="{ color: 'var(--text-primary)' }">文章</h3>
        <p class="text-sm" :style="{ color: 'var(--text-muted)' }">阅读最新技术分享与随笔</p>
      </router-link>
      <router-link to="/gallery" class="card-cyber p-6 group">
        <div class="w-12 h-12 rounded-xl flex items-center justify-center mb-4 group-hover:scale-110 transition-transform"
          style="background: linear-gradient(135deg, rgba(0,255,65,0.1), rgba(0,255,65,0.03)); color: var(--color-accent);">
          <svg class="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 16l4.586-4.586a2 2 0 012.828 0L16 16m-2-2l1.586-1.586a2 2 0 012.828 0L20 14m-6-6h.01M6 20h12a2 2 0 002-2V6a2 2 0 00-2-2H6a2 2 0 00-2 2v12a2 2 0 002 2z"/></svg>
        </div>
        <h3 class="text-lg font-semibold mb-1" :style="{ color: 'var(--text-primary)' }">时光剪影</h3>
        <p class="text-sm" :style="{ color: 'var(--text-muted)' }">浏览生活记录与精彩瞬间</p>
      </router-link>
      <router-link to="/downloads" class="card-cyber p-6 group">
        <div class="w-12 h-12 rounded-xl flex items-center justify-center mb-4 group-hover:scale-110 transition-transform"
          style="background: linear-gradient(135deg, rgba(255,171,0,0.1), rgba(255,171,0,0.03)); color: var(--color-warning);">
          <svg class="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 10v6m0 0l-3-3m3 3l3-3m2 8H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z"/></svg>
        </div>
        <h3 class="text-lg font-semibold mb-1" :style="{ color: 'var(--text-primary)' }">下载</h3>
        <p class="text-sm" :style="{ color: 'var(--text-muted)' }">获取实用资源与文件</p>
      </router-link>
    </div>

    <!-- Recent posts -->
    <section v-if="recentPosts.length">
      <h2 class="text-xl font-bold mb-5 flex items-center gap-2" :style="{ color: 'var(--text-primary)' }">
        <span class="inline-block w-1.5 h-5 rounded-full" :style="{ background: 'var(--color-primary)' }" />
        最近更新
      </h2>
      <div class="grid grid-cols-1 sm:grid-cols-2 gap-4">
        <router-link
          v-for="post in recentPosts"
          :key="post.slug"
          :to="`/posts/${post.slug}`"
          class="card-cyber p-5 flex flex-col justify-between"
        >
          <div>
            <div class="flex items-center gap-2 mb-2">
              <span class="badge-cyber" v-if="post.category">{{ post.category }}</span>
              <span class="text-xs" :style="{ color: 'var(--text-light)' }">{{ post.date }}</span>
            </div>
            <h3 class="font-semibold mb-1 group-hover:text-[var(--color-primary)] transition-colors" :style="{ color: 'var(--text-primary)' }">{{ post.title }}</h3>
            <p v-if="post.excerpt" class="text-sm line-clamp-2" :style="{ color: 'var(--text-muted)' }">{{ post.excerpt }}</p>
          </div>
          <div v-if="post.tags?.length" class="flex flex-wrap gap-1.5 mt-3">
            <span v-for="tag in post.tags" :key="tag" class="tag-cyber">{{ tag }}</span>
          </div>
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
  if (h < 12) return '早上好！'
  if (h < 14) return '中午好！'
  if (h < 18) return '下午好！'
  return '晚上好！'
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
      recentPosts.value = all.slice(0, 4)
    }
  } catch {}
})
</script>
