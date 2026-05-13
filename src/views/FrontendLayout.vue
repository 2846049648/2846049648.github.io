<template>
  <div class="min-h-screen bg-gray-50 flex flex-col">
    <header class="bg-white shadow-sm">
      <div class="max-w-4xl mx-auto px-4 py-6 flex justify-between items-center">
        <router-link to="/" class="text-2xl font-bold text-gray-900">我的个人空间</router-link>
        <nav class="space-x-6">
          <router-link to="/" class="text-gray-600 hover:text-blue-600">首页</router-link>
          <router-link to="/posts" class="text-gray-600 hover:text-blue-600">文章</router-link>
          <router-link to="/downloads" class="text-gray-600 hover:text-blue-600">下载</router-link>
          <router-link to="/about" class="text-gray-600 hover:text-blue-600">关于</router-link>
          <router-link v-if="isDev" to="/admin" class="text-orange-500 hover:text-orange-600 font-medium">管理</router-link>
        </nav>
      </div>
    </header>
    <main class="max-w-4xl mx-auto px-4 py-8 flex-1 w-full">
      <router-view />
    </main>
    <footer class="text-center text-gray-500 text-sm py-8 space-y-1">
      <p>&copy; {{ new Date().getFullYear() }} My Personal Site</p>
      <p v-if="isDev" class="text-xs text-orange-400">开发者模式 — <router-link to="/admin" class="underline">进入管理面板</router-link></p>
    </footer>
  </div>
</template>

<script setup>
import { computed } from 'vue'
const isDev = computed(() => import.meta.env.DEV)
</script>
