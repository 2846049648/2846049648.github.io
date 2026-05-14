<template>
  <div>
    <div class="flex justify-between items-center mb-6">
      <h2 class="text-2xl font-bold">文件库</h2>
      <el-upload
        :action="uploadUrl"
        :show-file-list="false"
        :on-success="handleSuccess"
        :on-error="handleError"
      >
        <el-button type="primary">上传文件</el-button>
      </el-upload>
    </div>
    <div v-if="loading" class="text-center py-12 text-gray-500">加载中...</div>
    <el-table :data="files" style="width: 100%" v-else-if="files.length > 0">
      <el-table-column prop="name" label="文件名" />
      <el-table-column prop="size" label="大小" width="120" />
      <el-table-column label="操作" width="220">
        <template #default="{ row }">
          <el-button size="small" @click="copyLink(row.url)">复制链接</el-button>
          <a :href="row.url" download>
            <el-button size="small">下载</el-button>
          </a>
          <el-button size="small" type="danger" @click="remove(row.name)">删除</el-button>
        </template>
      </el-table-column>
    </el-table>
    <div v-else class="text-gray-500 text-center py-12">
      暂无文件，点击上方按钮上传
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'

const files = ref([])
const loading = ref(true)
const uploadUrl = '/api/upload/file'

onMounted(loadFiles)

async function loadFiles() {
  loading.value = true
  try {
    const res = await fetch('/api/files')
    files.value = await res.json()
  } catch {
    files.value = []
  } finally {
    loading.value = false
  }
}

function handleSuccess(res) {
  if (res.url) {
    ElMessage.success('上传成功')
    loadFiles()
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
    const res = await fetch(`/api/files/${name}`, { method: 'DELETE' })
    if (res.ok) {
      ElMessage.success('已删除')
      loadFiles()
    }
  } catch {
    ElMessage.error('删除失败')
  }
}
</script>
