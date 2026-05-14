<template>
  <div>
    <h2 class="text-3xl font-bold mb-2" :style="{ color: 'var(--text-primary)' }">资源下载</h2>
    <p class="mb-8" :style="{ color: 'var(--text-muted)' }">实用资源与文件下载</p>

    <div v-if="loading" class="text-center py-16" :style="{ color: 'var(--text-light)' }">
      <svg class="animate-spin w-8 h-8 mx-auto mb-3" fill="none" viewBox="0 0 24 24"><circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"/><path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4z"/></svg>
      加载中...
    </div>
    <div v-else-if="files.length === 0" class="text-center py-16" :style="{ color: 'var(--text-light)' }">
      <svg class="w-12 h-12 mx-auto mb-3" :style="{ color: 'var(--text-light)' }" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="1.5" d="M12 10v6m0 0l-3-3m3 3l3-3m2 8H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z"/></svg>
      暂无可用文件
    </div>
    <div v-else class="grid gap-4">
      <div
        v-for="file in files" :key="file.name"
        class="group rounded-xl p-5 shadow-sm hover:shadow-md transition-all border flex items-center justify-between"
        :style="{ background: 'var(--bg-surface)', borderColor: 'var(--border-color)' }"
      >
        <div class="flex items-center space-x-4">
          <div class="w-10 h-10 rounded-lg bg-gradient-to-br from-blue-500 to-blue-600 flex items-center justify-center text-white shadow-sm">
            <svg class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 10v6m0 0l-3-3m3 3l3-3m2 8H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z"/></svg>
          </div>
          <div>
            <p class="font-medium" :style="{ color: 'var(--text-primary)' }">{{ file.name }}</p>
            <p class="text-sm" :style="{ color: 'var(--text-light)' }">{{ file.size }}</p>
          </div>
        </div>
        <a :href="file.url" download
          class="px-4 py-2 rounded-lg text-sm font-medium transition-colors"
          :style="{ background: 'var(--bg-muted)', color: 'var(--color-primary)' }">
          下载
        </a>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'

const files = ref([])
const loading = ref(true)

onMounted(async () => {
  try {
    const url = import.meta.env.DEV ? '/api/files' : '/files/index.json'
    const res = await fetch(url)
    files.value = await res.json()
  } catch {
    files.value = []
  } finally {
    loading.value = false
  }
})
</script>
