<template>
  <div>
    <h2 class="text-2xl font-bold mb-6">管理概览</h2>
    <div class="grid grid-cols-1 md:grid-cols-3 gap-6 mb-8">
      <el-card>
        <p class="text-gray-500 text-sm">文章总数</p>
        <p class="text-3xl font-bold mt-2">{{ stats.posts }}</p>
      </el-card>
      <el-card>
        <p class="text-gray-500 text-sm">媒体文件</p>
        <p class="text-3xl font-bold mt-2">{{ stats.media }}</p>
      </el-card>
      <el-card>
        <p class="text-gray-500 text-sm">下载文件</p>
        <p class="text-3xl font-bold mt-2">{{ stats.files }}</p>
      </el-card>
    </div>
    <el-alert
      title="管理面板仅在本地开发环境可用"
      type="info"
      :closable="false"
      show-icon
    />
    <div class="mt-6 p-6 bg-white rounded-lg shadow-sm">
      <h3 class="text-lg font-semibold mb-4">一键发布</h3>
      <p class="text-gray-600 text-sm mb-4">执行构建并推送至 GitHub Pages</p>
      <el-button type="primary" size="large" :loading="publishing" @click="publish">
        {{ publishing ? '发布中...' : '一键发布到 GitHub Pages' }}
      </el-button>
    </div>
  </div>
</template>

<script setup>
import { reactive, ref, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'

const stats = reactive({ posts: 0, media: 0, files: 0 })
const publishing = ref(false)

onMounted(async () => {
  try {
    const res = await fetch('/api/stats')
    if (res.ok) Object.assign(stats, await res.json())
  } catch {}
})

async function publish() {
  try {
    await ElMessageBox.confirm('将执行 npm run build 并 git push，确认发布？', '确认', {
      confirmButtonText: '发布',
      cancelButtonText: '取消',
      type: 'warning',
    })
  } catch { return }

  publishing.value = true
  try {
    const res = await fetch('/api/publish', { method: 'POST' })
    const data = await res.json()
    if (data.ok) {
      ElMessage.success('发布成功！网站已更新')
    } else {
      ElMessage.error('发布失败: ' + (data.error || '未知错误'))
    }
  } catch (e) {
    ElMessage.error('发布失败: ' + e.message)
  } finally {
    publishing.value = false
  }
}
</script>
