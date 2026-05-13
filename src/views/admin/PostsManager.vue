<template>
  <div>
    <div class="flex justify-between items-center mb-6">
      <h2 class="text-2xl font-bold">文章管理</h2>
      <router-link to="/admin/posts/new">
        <el-button type="primary">写新文章</el-button>
      </router-link>
    </div>
    <el-table :data="posts" style="width: 100%" v-loading="loading">
      <el-table-column prop="title" label="标题" min-width="200" />
      <el-table-column prop="date" label="日期" width="120" />
      <el-table-column prop="category" label="分类" width="100" />
      <el-table-column label="标签" width="200">
        <template #default="{ row }">
          <el-tag v-for="tag in row.tags" :key="tag" size="small" class="mr-1">{{ tag }}</el-tag>
        </template>
      </el-table-column>
      <el-table-column label="操作" width="150">
        <template #default="{ row }">
          <el-button size="small" @click="edit(row.slug)">编辑</el-button>
          <el-button size="small" type="danger" :loading="deleting === row.slug" @click="remove(row.slug)">删除</el-button>
        </template>
      </el-table-column>
    </el-table>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { ElMessage, ElMessageBox } from 'element-plus'

const router = useRouter()
const posts = ref([])
const loading = ref(true)
const deleting = ref('')

onMounted(loadPosts)

async function loadPosts() {
  loading.value = true
  try {
    const res = await fetch('/api/posts')
    posts.value = await res.json()
  } catch {
    posts.value = []
  } finally {
    loading.value = false
  }
}

function edit(slug) {
  router.push(`/admin/posts/${slug}/edit`)
}

async function remove(slug) {
  try {
    await ElMessageBox.confirm('确定要删除这篇文章吗？此操作不可恢复。', '确认删除', { type: 'warning' })
  } catch { return }

  deleting.value = slug
  try {
    const res = await fetch(`/api/posts/${slug}`, { method: 'DELETE' })
    if (res.ok) {
      ElMessage.success('已删除')
      await loadPosts()
    }
  } catch (e) {
    ElMessage.error('删除失败: ' + e.message)
  } finally {
    deleting.value = ''
  }
}
</script>
