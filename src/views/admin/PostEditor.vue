<template>
  <div>
    <h2 class="text-2xl font-bold mb-6">{{ isEdit ? '编辑文章' : '写新文章' }}</h2>
    <el-form label-width="80">
      <el-form-item label="标识">
        <el-input v-model="slug" placeholder="url 标识（如 my-first-post）" :disabled="isEdit" />
      </el-form-item>
      <el-form-item label="标题">
        <el-input v-model="title" placeholder="文章标题" />
      </el-form-item>
      <el-form-item label="分类">
        <el-input v-model="category" placeholder="分类名称" />
      </el-form-item>
      <el-form-item label="标签">
        <el-select v-model="tags" multiple filterable allow-create default-first-option
          placeholder="输入标签后回车" style="width: 100%">
          <el-option v-for="tag in tags" :key="tag" :label="tag" :value="tag" />
        </el-select>
      </el-form-item>
      <el-form-item label="摘要">
        <el-input v-model="excerpt" type="textarea" :rows="3" placeholder="文章摘要（可选）" />
      </el-form-item>
      <el-form-item label="内容">
        <div class="border rounded w-full">
          <div class="flex border-b bg-gray-50">
            <el-tabs v-model="activeTab" class="px-2">
              <el-tab-pane label="编辑" name="edit" />
              <el-tab-pane label="预览" name="preview" />
            </el-tabs>
          </div>
          <el-input
            v-if="activeTab === 'edit'"
            v-model="content"
            type="textarea"
            :rows="20"
            placeholder="使用 Markdown 编写文章..."
            class="border-0 font-mono"
          />
          <div v-else class="p-4 prose max-w-none" v-html="previewHtml" />
        </div>
      </el-form-item>
      <el-form-item>
        <el-button type="primary" :loading="saving" @click="save">{{ isEdit ? '更新' : '保存' }}</el-button>
        <el-button @click="cancel">取消</el-button>
      </el-form-item>
    </el-form>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { ElMessage } from 'element-plus'
import { marked } from 'marked'

const route = useRoute()
const router = useRouter()
const isEdit = computed(() => !!route.params.slug)

const slug = ref('')
const title = ref('')
const category = ref('')
const tags = ref([])
const excerpt = ref('')
const content = ref('')
const activeTab = ref('edit')
const saving = ref(false)

const previewHtml = computed(() => {
  try {
    return marked(content.value || '')
  } catch {
    return content.value
  }
})

onMounted(async () => {
  if (isEdit.value) {
    try {
      const res = await fetch(`/api/posts/${route.params.slug}`)
      if (res.ok) {
        const data = await res.json()
        slug.value = data.slug
        title.value = data.title || ''
        category.value = data.category || ''
        tags.value = data.tags || []
        excerpt.value = data.excerpt || ''
        content.value = data.content || ''
      }
    } catch (e) {
      ElMessage.error('加载文章失败: ' + e.message)
    }
  }
})

async function save() {
  if (!slug.value) { ElMessage.warning('请输入文章标识'); return }
  if (!title.value) { ElMessage.warning('请输入文章标题'); return }

  saving.value = true
  try {
    const res = await fetch('/api/posts', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        slug: slug.value,
        title: title.value,
        date: new Date().toISOString().split('T')[0],
        category: category.value,
        tags: tags.value,
        excerpt: excerpt.value,
        content: content.value,
      }),
    })
    const data = await res.json()
    if (data.ok) {
      ElMessage.success(isEdit.value ? '文章已更新' : '文章已创建')
      router.push('/admin/posts')
    } else {
      ElMessage.error('保存失败: ' + (data.error || '未知错误'))
    }
  } catch (e) {
    ElMessage.error('保存失败: ' + e.message)
  } finally {
    saving.value = false
  }
}

function cancel() {
  router.push('/admin/posts')
}
</script>
