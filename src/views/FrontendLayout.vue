<template>
  <div class="min-h-screen flex flex-col" :class="{ 'bg-[var(--bg-body)]': true }">
    <!-- Sticky Header -->
    <header class="sticky top-0 z-50" :style="{ background: 'var(--header-bg)', borderBottom: '1px solid var(--header-border)' }">
      <div class="max-w-5xl mx-auto px-4 sm:px-6 h-16 flex items-center justify-between">
        <router-link to="/" class="text-xl font-bold bg-gradient-to-r from-blue-600 to-purple-600 bg-clip-text text-transparent">
          {{ profile.name || '我的空间' }}
        </router-link>
        <nav class="flex items-center space-x-1 sm:space-x-2">
          <router-link to="/" class="nav-link">首页</router-link>
          <router-link to="/posts" class="nav-link">文章</router-link>
          <router-link to="/gallery" class="nav-link">时光剪影</router-link>
          <router-link to="/downloads" class="nav-link">下载</router-link>
          <router-link to="/about" class="nav-link">关于</router-link>
          <router-link v-if="isDev" to="/admin" class="nav-link text-orange-500 font-medium">管理</router-link>
          <!-- Search button -->
          <button
            @click="showSearch = true"
            class="p-2 rounded-lg transition-all"
            :class="isDark ? 'text-gray-400 hover:bg-gray-700' : 'text-gray-500 hover:bg-gray-100'"
            title="搜索文章"
          >
            <svg class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0z"/>
            </svg>
          </button>
          <!-- Dark mode toggle -->
          <button
            @click="toggleDark"
            class="p-2 rounded-lg transition-all"
            :class="isDark ? 'text-yellow-400 hover:bg-gray-700' : 'text-gray-500 hover:bg-gray-100'"
            :title="isDark ? '切换亮色模式' : '切换暗色模式'"
          >
            <svg v-if="isDark" class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 3v1m0 16v1m9-9h-1M4 12H3m15.364 6.364l-.707-.707M6.343 6.343l-.707-.707m12.728 0l-.707.707M6.343 17.657l-.707.707M16 12a4 4 0 11-8 0 4 4 0 018 0z"/>
            </svg>
            <svg v-else class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M20.354 15.354A9 9 0 018.646 3.646 9.003 9.003 0 0012 21a9.003 9.003 0 008.354-5.646z"/>
            </svg>
          </button>
        </nav>
      </div>
    </header>

    <!-- Main Content -->
    <main class="flex-1 w-full max-w-5xl mx-auto px-4 sm:px-6 py-8">
      <router-view />
    </main>

    <!-- Footer -->
    <footer class="border-t" :style="{ borderColor: 'var(--border-color)', background: 'var(--bg-surface)' }">
      <div class="max-w-5xl mx-auto px-4 sm:px-6 py-8 text-center text-sm space-y-1" :style="{ color: 'var(--text-light)' }">
        <p>&copy; {{ new Date().getFullYear() }} {{ profile.name || 'My Site' }}. All rights reserved.</p>
        <p v-if="isDev">
          <router-link to="/admin" class="hover:underline" :style="{ color: '#f97316' }">开发者模式 — 进入管理面板</router-link>
        </p>
      </div>
    </footer>

    <!-- Search Modal -->
    <SearchModal :visible="showSearch" @close="showSearch = false" />
  </div>
</template>

<script setup>
import { computed, ref, onMounted } from 'vue'
import { useDarkMode } from '../composables/useDarkMode.js'
import SearchModal from '../components/SearchModal.vue'

const isDev = computed(() => import.meta.env.DEV)
const profile = ref({ name: '我的空间' })
const { isDark, toggle: toggleDark } = useDarkMode()
const showSearch = ref(false)

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
  transition: all 0.2s;
}
.nav-link:hover {
  background: var(--bg-surface-hover);
}
.nav-link.router-link-active {
  color: #2563eb;
  background: #eff6ff;
  font-weight: 500;
}
.dark .nav-link.router-link-active {
  background: rgba(37, 99, 235, 0.15);
  color: #60a5fa;
}
</style>
