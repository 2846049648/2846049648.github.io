<template>
  <div>
    <h2 class="text-3xl font-bold text-gray-900 mb-2">资源下载</h2>
    <p class="text-gray-500 mb-8">实用资源与文件下载</p>

    <div v-if="loading" class="text-center py-16 text-gray-400">
      <svg class="animate-spin w-8 h-8 mx-auto mb-3" fill="none" viewBox="0 0 24 24"><circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"/><path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4z"/></svg>
      加载中...
    </div>
    <div v-else-if="files.length === 0" class="text-center py-16 text-gray-400">
      <svg class="w-12 h-12 mx-auto mb-3 text-gray-300" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="1.5" d="M12 10v6m0 0l-3-3m3 3l3-3m2 8H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z"/></svg>
      暂无可用文件
    </div>
    <div v-else class="grid gap-4">
      <div
        v-for="file in files" :key="file.name"
        class="group bg-white rounded-xl p-5 shadow-sm hover:shadow-md transition-all border border-gray-100 flex items-center justify-between"
      >
        <div class="flex items-center space-x-4">
          <div class="w-10 h-10 rounded-lg bg-gradient-to-br from-blue-500 to-blue-600 flex items-center justify-center text-white shadow-sm">
            <svg class="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 10v6m0 0l-3-3m3 3l3-3m2 8H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z"/></svg>
          </div>
          <div>
            <p class="font-medium text-gray-900">{{ file.name }}</p>
            <p class="text-sm text-gray-400">{{ file.size }}</p>
          </div>
        </div>
        <a :href="file.url" download
          class="px-4 py-2 rounded-lg bg-blue-50 text-blue-600 hover:bg-blue-100 transition-colors text-sm font-medium">
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
    const res = await fetch('/api/files')
    files.value = await res.json()
  } catch {
    files.value = []
  } finally {
    loading.value = false
  }
})
</script>
