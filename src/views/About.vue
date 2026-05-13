<template>
  <div class="max-w-2xl mx-auto">
    <div class="bg-white rounded-xl shadow-sm p-8 text-center">
      <div class="w-28 h-28 mx-auto mb-4 rounded-full overflow-hidden bg-gray-200">
        <img v-if="profile.avatar" :src="profile.avatar" alt="avatar" class="w-full h-full object-cover" />
        <div v-else class="w-full h-full flex items-center justify-center text-4xl text-gray-400">
          {{ profile.name.charAt(0) }}
        </div>
      </div>
      <h1 class="text-3xl font-bold text-gray-900 mb-2">{{ profile.name }}</h1>
      <p class="text-gray-600 mb-6">{{ profile.bio }}</p>

      <div v-if="profile.email || profile.social" class="flex justify-center space-x-4 mb-8">
        <a v-if="profile.email" :href="'mailto:' + profile.email" class="text-gray-500 hover:text-blue-600">
          <svg class="w-6 h-6 inline" fill="none" stroke="currentColor" viewBox="0 0 24 24">
            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 8l7.89 5.26a2 2 0 002.22 0L21 8M5 19h14a2 2 0 002-2V7a2 2 0 00-2-2H5a2 2 0 00-2 2v10a2 2 0 002 2z" />
          </svg>
        </a>
        <a v-for="(url, platform) in profile.social" :key="platform" :href="url" target="_blank" class="text-gray-500 hover:text-blue-600">
          {{ platform }}
        </a>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'

const profile = ref({
  name: '作者',
  avatar: '',
  bio: '这个人很懒，什么都没写…',
  email: '',
  social: {},
})

onMounted(async () => {
  try {
    const res = await fetch('/api/profile')
    if (res.ok) profile.value = await res.json()
  } catch {}
})
</script>
