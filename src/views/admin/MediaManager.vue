<template>
  <div>
    <div class="flex justify-between items-center mb-6">
      <h2 class="text-2xl font-bold">媒体库</h2>
      <el-upload
        :action="uploadUrl"
        :show-file-list="false"
        :on-success="handleSuccess"
        :on-error="handleError"
        accept="image/*"
      >
        <el-button type="primary">上传图片</el-button>
      </el-upload>
    </div>
    <div v-if="loading" class="text-center py-12" :style="{ color: 'var(--text-light)' }">加载中...</div>
    <div v-else-if="images.length === 0" class="text-center py-12" :style="{ color: 'var(--text-light)' }">
      暂无图片，点击上方按钮上传
    </div>
    <div v-else class="grid grid-cols-2 md:grid-cols-4 gap-4">
      <div v-for="img in images" :key="img.name" class="relative group border rounded-lg overflow-hidden" :style="{ background: 'var(--bg-surface)', borderColor: 'var(--border-color)' }">
        <img :src="img.url" :alt="img.name" class="w-full h-40 object-cover" />
        <div class="absolute inset-0 bg-black/50 opacity-0 group-hover:opacity-100 transition-opacity flex items-center justify-center gap-2">
          <el-button size="small" type="primary" @click="copyLink(img.url)">复制链接</el-button>
          <el-button size="small" type="danger" @click="remove(img.name)">删除</el-button>
        </div>
        <p class="text-xs truncate p-1" :style="{ color: 'var(--text-muted)' }">{{ img.name }}</p>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'

const images = ref([])
const loading = ref(true)
const uploadUrl = '/api/upload/image'

onMounted(loadMedia)

async function loadMedia() {
  loading.value = true
  try {
    const res = await fetch('/api/media')
    images.value = await res.json()
  } catch {
    images.value = []
  } finally {
    loading.value = false
  }
}

function handleSuccess(res) {
  if (res.url) {
    ElMessage.success('上传成功')
    images.value.unshift({ name: res.url.split('/').pop(), url: res.url })
  }
}

function handleError() {
  ElMessage.error('上传失败')
}

function copyLink(url) {
  navigator.clipboard.writeText(window.location.origin + url)
  ElMessage.success('链接已复制')
}

async function remove(name) {
  try {
    await ElMessageBox.confirm('确认删除此文件？', '确认', { type: 'warning' })
  } catch { return }
  try {
    const res = await fetch(`/api/media/${name}`, { method: 'DELETE' })
    if (res.ok) {
      ElMessage.success('已删除')
      loadMedia()
    }
  } catch {
    ElMessage.error('删除失败')
  }
}
</script>
