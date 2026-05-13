<template>
  <div>
    <div class="flex justify-between items-center mb-6">
      <h2 class="text-2xl font-bold">相册管理</h2>
      <el-button type="primary" @click="showCreate">新建相册</el-button>
    </div>

    <!-- Album list -->
    <div v-if="loading" class="text-center py-12 text-gray-400">加载中...</div>
    <div v-else-if="albums.length === 0" class="text-center py-12 text-gray-400">暂无相册，点击新建</div>
    <div v-else class="grid grid-cols-1 md:grid-cols-2 gap-4">
      <el-card v-for="album in albums" :key="album.name" class="!p-0" shadow="sm">
        <div class="p-4">
          <div class="flex justify-between items-start mb-3">
            <div>
              <h3 class="font-semibold text-gray-900">{{ album.title }}</h3>
              <p class="text-sm text-gray-500">{{ album.photos?.length || 0 }} 张照片</p>
            </div>
            <el-button type="danger" size="small" :loading="deleting === album.name" @click="remove(album.name)">删除</el-button>
          </div>
          <div class="text-xs text-gray-400 mb-2">{{ album.description || '无描述' }}</div>
          <!-- Photos preview -->
          <div v-if="album.photos?.length" class="flex gap-1 flex-wrap mb-3">
            <img v-for="(p, i) in album.photos.slice(0, 6)" :key="i" :src="p"
              class="w-14 h-14 object-cover rounded cursor-pointer border-2 hover:border-blue-400 transition-all"
              @click="previewImage = p" />
          </div>
          <!-- Upload to this album -->
          <el-upload
            :action="uploadUrl"
            :data="{ album: album.name }"
            :show-file-list="false"
            :on-success="() => reload()"
            :on-error="() => ElMessage.error('上传失败')"
            accept="image/*"
          >
            <el-button size="small">上传照片</el-button>
          </el-upload>
        </div>
      </el-card>
    </div>

    <!-- Create/Edit dialog -->
    <el-dialog v-model="dialogVisible" :title="editing ? '编辑相册' : '新建相册'" width="480">
      <el-form label-width="80">
        <el-form-item label="标识">
          <el-input v-model="form.name" placeholder="英文标识（如 travel-2026）" :disabled="!!editing" />
        </el-form-item>
        <el-form-item label="标题">
          <el-input v-model="form.title" placeholder="相册标题" />
        </el-form-item>
        <el-form-item label="描述">
          <el-input v-model="form.description" type="textarea" :rows="2" placeholder="描述（可选）" />
        </el-form-item>
        <el-form-item label="封面">
          <el-input v-model="form.cover" placeholder="封面图片 URL（选填，默认用第一张）" />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="dialogVisible = false">取消</el-button>
        <el-button type="primary" :loading="saving" @click="save">保存</el-button>
      </template>
    </el-dialog>

    <!-- Preview modal -->
    <div v-if="previewImage" class="fixed inset-0 z-[999] bg-black/80 flex items-center justify-center cursor-pointer" @click="previewImage = ''">
      <img :src="previewImage" class="max-w-[90vw] max-h-[90vh] object-contain" @click.stop />
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'

const albums = ref([])
const loading = ref(true)
const deleting = ref('')
const dialogVisible = ref(false)
const editing = ref(false)
const saving = ref(false)
const previewImage = ref('')
const uploadUrl = '/api/upload/photo'

const form = ref({ name: '', title: '', description: '', cover: '', photos: [] })

onMounted(reload)

async function reload() {
  loading.value = true
  try {
    const res = await fetch('/api/albums')
    albums.value = await res.json()
  } catch { albums.value = [] }
  finally { loading.value = false }
}

function showCreate() {
  editing.value = false
  form.value = { name: '', title: '', description: '', cover: '', photos: [] }
  dialogVisible.value = true
}

async function save() {
  if (!form.value.name || !form.value.title) {
    ElMessage.warning('请填写标识和标题')
    return
  }
  saving.value = true
  try {
    const albumDir = `/images/gallery/${form.value.name}`
    const payload = {
      ...form.value,
      cover: form.value.cover || `${albumDir}/placeholder.jpg`,
    }

    const res = await fetch('/api/albums', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload),
    })
    if (res.ok) {
      ElMessage.success('已保存')
      dialogVisible.value = false
      await reload()
    }
  } catch (e) {
    ElMessage.error('保存失败: ' + e.message)
  } finally {
    saving.value = false
  }
}

async function remove(name) {
  try {
    await ElMessageBox.confirm('删除相册将同时删除所有照片，确认？', '确认删除', { type: 'warning' })
  } catch { return }
  deleting.value = name
  try {
    await fetch(`/api/albums/${name}`, { method: 'DELETE' })
    ElMessage.success('已删除')
    await reload()
  } catch (e) {
    ElMessage.error('删除失败: ' + e.message)
  } finally {
    deleting.value = ''
  }
}
</script>
