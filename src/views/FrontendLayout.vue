<template>
  <div class="min-h-screen bg-gray-50 flex flex-col">
    <!-- Sticky Header -->
    <header class="sticky top-0 z-50 bg-white/80 backdrop-blur-md border-b border-gray-100">
      <div class="max-w-5xl mx-auto px-4 sm:px-6 h-16 flex items-center justify-between">
        <router-link to="/" class="text-xl font-bold bg-gradient-to-r from-blue-600 to-purple-600 bg-clip-text text-transparent">
          {{ profile.name || '我的空间' }}
        </router-link>
        <nav class="flex items-center space-x-1 sm:space-x-2">
          <router-link to="/" class="nav-link">首页</router-link>
          <router-link to="/posts" class="nav-link">文章</router-link>
          <router-link to="/gallery" class="nav-link">相册</router-link>
          <router-link to="/downloads" class="nav-link">下载</router-link>
          <router-link to="/about" class="nav-link">关于</router-link>
          <router-link v-if="isDev" to="/admin" class="nav-link text-orange-500 font-medium">管理</router-link>
        </nav>
      </div>
    </header>

    <!-- Main Content -->
    <main class="flex-1 w-full max-w-5xl mx-auto px-4 sm:px-6 py-8">
      <router-view />
    </main>

    <!-- Footer -->
    <footer class="border-t border-gray-100 bg-white">
      <div class="max-w-5xl mx-auto px-4 sm:px-6 py-8 text-center text-sm text-gray-400 space-y-1">
        <p>&copy; {{ new Date().getFullYear() }} {{ profile.name || 'My Site' }}. All rights reserved.</p>
        <p v-if="isDev" class="text-orange-400">
          <router-link to="/admin" class="hover:underline">开发者模式 — 进入管理面板</router-link>
        </p>
      </div>
    </footer>
  </div>
</template>

<script setup>
import { computed, ref, onMounted } from 'vue'
const isDev = computed(() => import.meta.env.DEV)
const profile = ref({ name: '我的空间' })
onMounted(async () => {
  try {
    const url = import.meta.env.DEV ? '/api/profile' : '/profile.json'
    const res = await fetch(url)
    if (res.ok) profile.value = await res.json()
  } catch {}
})
</script>

<style scoped>
.nav-link {
  padding: 0.5rem 0.75rem;
  border-radius: 0.5rem;
  font-size: 0.875rem;
  color: #6b7280;
  transition: all 0.2s;
}
.nav-link:hover {
  color: #1f2937;
  background: #f3f4f6;
}
.nav-link.router-link-active {
  color: #2563eb;
  background: #eff6ff;
  font-weight: 500;
}
</style>
