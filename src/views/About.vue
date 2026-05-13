<template>
  <div class="max-w-2xl mx-auto">
    <div class="bg-white rounded-2xl shadow-sm border border-gray-100 p-8 sm:p-10 text-center">
      <!-- Avatar -->
      <div class="w-28 h-28 mx-auto mb-5 rounded-full bg-gradient-to-br from-blue-500 to-purple-600 flex items-center justify-center text-4xl text-white font-bold ring-4 ring-blue-50 overflow-hidden">
        <img v-if="profile.avatar" :src="profile.avatar" class="w-full h-full object-cover" />
        <span v-else>{{ (profile.name || '?').charAt(0) }}</span>
      </div>

      <!-- Name & Bio -->
      <h1 class="text-3xl font-bold text-gray-900 mb-2">{{ profile.name }}</h1>
      <p class="text-gray-500 mb-6 max-w-md mx-auto">{{ profile.bio }}</p>

      <!-- Divider -->
      <div class="w-16 h-0.5 bg-gradient-to-r from-blue-500 to-purple-600 mx-auto mb-6 rounded-full" />

      <!-- Contact & Social -->
      <div v-if="profile.email || Object.keys(profile.social || {}).length" class="flex justify-center flex-wrap gap-4">
        <a v-if="profile.email" :href="'mailto:' + profile.email"
          class="inline-flex items-center gap-2 px-4 py-2 rounded-lg bg-gray-50 text-gray-600 hover:bg-blue-50 hover:text-blue-600 transition-all text-sm">
          <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 8l7.89 5.26a2 2 0 002.22 0L21 8M5 19h14a2 2 0 002-2V7a2 2 0 00-2-2H5a2 2 0 00-2 2v10a2 2 0 002 2z"/></svg>
          发送邮件
        </a>
        <a v-for="(url, platform) in profile.social" :key="platform" :href="url" target="_blank"
          class="inline-flex items-center gap-2 px-4 py-2 rounded-lg bg-gray-50 text-gray-600 hover:bg-gray-100 transition-all text-sm">
          <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M10 6H6a2 2 0 00-2 2v10a2 2 0 002 2h10a2 2 0 002-2v-4M14 4h6m0 0v6m0-6L10 14"/></svg>
          {{ platform }}
        </a>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'

const profile = ref({
  name: '作者', avatar: '',
  bio: '这个人很懒，什么都没写…',
  email: '', social: {},
})

onMounted(async () => {
  try {
    const url = import.meta.env.DEV ? '/api/profile' : '/profile.json'
    const res = await fetch(url)
    if (res.ok) profile.value = await res.json()
  } catch {}
})
</script>
