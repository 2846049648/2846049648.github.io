<template>
  <div>
    <h2 class="text-3xl font-bold mb-6">资源下载</h2>
    <div v-if="loading" class="text-center py-12 text-gray-500">加载中...</div>
    <div v-else-if="files.length === 0" class="text-center py-12 text-gray-500">暂无可用文件</div>
    <div v-else class="grid gap-4">
      <div
        v-for="file in files"
        :key="file.name"
        class="bg-white rounded-lg shadow p-4 flex items-center justify-between hover:shadow-md transition-shadow"
      >
        <div class="flex items-center space-x-3">
          <svg class="w-8 h-8 text-gray-400" fill="none" stroke="currentColor" viewBox="0 0 24 24">
            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2"
              d="M12 10v6m0 0l-3-3m3 3l3-3m2 8H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z" />
          </svg>
          <div>
            <p class="font-medium text-gray-900">{{ file.name }}</p>
            <p class="text-sm text-gray-500">{{ file.size }}</p>
          </div>
        </div>
        <a :href="file.url" download
          class="bg-blue-600 text-white px-4 py-2 rounded-lg hover:bg-blue-700 transition-colors text-sm">
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
