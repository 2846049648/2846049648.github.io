<template>
  <div class="max-w-2xl mx-auto">
    <div class="card-cyber p-8 sm:p-10 text-center">
      <!-- Avatar with cyber ring -->
      <div class="w-28 h-28 mx-auto mb-5 rounded-full flex items-center justify-center text-4xl font-bold overflow-hidden"
        style="border: 2px solid rgba(0, 229, 255, 0.3); box-shadow: 0 0 30px rgba(0, 229, 255, 0.1); background: rgba(0, 229, 255, 0.03);">
        <img v-if="profile.avatar" :src="profile.avatar" class="w-full h-full object-cover" />
        <span v-else class="gradient-text">{{ (profile.name || '?').charAt(0) }}</span>
      </div>

      <!-- Name & Bio -->
      <h1 class="text-3xl font-bold mb-2" :style="{ color: 'var(--text-primary)' }">{{ profile.name }}</h1>
      <p class="mb-6 max-w-md mx-auto" :style="{ color: 'var(--text-muted)' }">{{ profile.bio }}</p>

      <!-- Divider -->
      <div class="w-16 h-0.5 mx-auto mb-6 rounded-full" style="background: linear-gradient(90deg, var(--color-primary), var(--color-accent));" />

      <!-- Skills -->
      <div v-if="profile.skills?.length" class="mb-6">
        <h3 class="text-sm font-semibold mb-3" :style="{ color: 'var(--text-muted)' }">技能栈</h3>
        <div class="flex flex-wrap justify-center gap-2">
          <span
            v-for="skill in profile.skills" :key="skill"
            class="tag-cyber"
          >{{ skill }}</span>
        </div>
      </div>

      <!-- Contact & Social -->
      <div v-if="profile.email || Object.keys(profile.social || {}).length" class="flex justify-center flex-wrap gap-4">
        <a v-if="profile.email" :href="'mailto:' + profile.email"
          class="btn-cyber btn-cyber-glass text-sm">
          <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 8l7.89 5.26a2 2 0 002.22 0L21 8M5 19h14a2 2 0 002-2V7a2 2 0 00-2-2H5a2 2 0 00-2 2v10a2 2 0 002 2z"/></svg>
          发送邮件
        </a>
        <a v-for="(url, platform) in profile.social" :key="platform" :href="url" target="_blank"
          class="btn-cyber btn-cyber-glass text-sm">
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
  email: '', social: {}, skills: [],
})

onMounted(async () => {
  try {
    const url = import.meta.env.DEV ? '/api/profile' : '/profile.json'
    const res = await fetch(url)
    if (res.ok) profile.value = await res.json()
  } catch {}
})
</script>
