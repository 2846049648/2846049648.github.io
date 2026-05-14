<template>
  <div>
    <div class="flex justify-between items-center mb-6">
      <h2 class="text-2xl font-bold">相册管理</h2>
      <el-button type="primary" @click="showCreate">新建相册</el-button>
    </div>

    <!-- Album list -->
    <div v-if="loading" class="text-center py-12" :style="{ color: 'var(--text-light)' }">加载中...</div>
    <div v-else-if="albums.length === 0" class="text-center py-12" :style="{ color: 'var(--text-light)' }">暂无相册，点击新建</div>
    <div v-else class="grid grid-cols-1 md:grid-cols-2 gap-4">
      <el-card v-for="album in albums" :key="album.name" class="!p-0" shadow="sm">
        <div class="p-4">
          <div class="flex justify-between items-start mb-3">
            <div>
              <h3 class="font-semibold" :style="{ color: 'var(--text-primary)' }">{{ album.title }}</h3>
              <p class="text-sm" :style="{ color: 'var(--text-muted)' }">{{ album.photos?.length || 0 }} 张照片</p>
              <p v-if="album.story" class="text-xs mt-1" style="color: #8b5cf6;">📝 包含摄影随笔</p>
            </div>
            <div class="flex gap-1">
              <el-button size="small" @click="editAlbum(album)">编辑</el-button>
              <el-button type="danger" size="small" :loading="deleting === album.name" @click="remove(album.name)">删除</el-button>
            </div>
          </div>
          <div class="text-xs mb-2" :style="{ color: 'var(--text-muted)' }">{{ album.description || '无描述' }}</div>
          <!-- Photos preview -->
          <div v-if="album.photos?.length" class="flex gap-1 flex-wrap mb-3">
            <div
              v-for="(p, i) in album.photos.slice(0, 6)" :key="i"
              class="relative group/item"
            >
              <img :src="getPhotoUrl(p)"
                class="w-14 h-14 object-cover rounded cursor-pointer border-2 hover:border-blue-400 transition-all"
                @click="previewImage = getPhotoUrl(p)" />
              <span v-if="getPhotoCaption(p)" class="absolute bottom-0 left-0 right-0 bg-black/50 text-white text-[8px] px-0.5 truncate opacity-0 group-hover/item:opacity-100 transition-opacity">
                {{ getPhotoCaption(p) }}
              </span>
            </div>
          </div>
          <!-- Upload to this album -->
          <div class="flex gap-2 flex-wrap">
            <el-upload
              :action="uploadUrl"
              :data="{ album: album.name }"
              :show-file-list="false"
              :on-success="onPhotoUploaded"
              :on-error="() => ElMessage.error('上传失败')"
              accept="image/*"
            >
              <el-button size="small">上传照片</el-button>
            </el-upload>
            <el-button size="small" @click="editPhotos(album)">管理照片</el-button>
          </div>
        </div>
      </el-card>
    </div>

    <!-- Create/Edit dialog -->
    <el-dialog v-model="dialogVisible" :title="editing ? '编辑相册' : '新建相册'" width="560">
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
        <el-form-item label="摄影随笔">
          <el-input
            v-model="form.story"
            type="textarea"
            :rows="4"
            placeholder="用 Markdown 讲述照片背后的故事（可选）"
          />
          <div class="text-xs mt-1" :style="{ color: 'var(--text-light)' }">支持 Markdown 格式</div>
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="dialogVisible = false">取消</el-button>
        <el-button type="primary" :loading="saving" @click="save">保存</el-button>
      </template>
    </el-dialog>

    <!-- Photo management dialog -->
    <el-dialog v-model="photoDialogVisible" :title="'管理照片 - ' + photoAlbumName" width="640">
      <div v-if="!photoAlbum" class="text-center py-4 text-gray-400">暂无数据</div>
      <div v-else class="space-y-3">
        <div
          v-for="(photo, idx) in photoAlbum.photos" :key="idx"
          class="flex items-center gap-3 p-2 rounded-lg border"
          :style="{ borderColor: 'var(--border-color)' }"
        >
          <img :src="getPhotoUrl(photo)" class="w-16 h-16 object-cover rounded flex-shrink-0" />
          <div class="flex-1 min-w-0">
            <el-input
              :model-value="getPhotoCaption(photo)"
              placeholder="照片说明（可选）"
              size="small"
              @input="(val) => updateCaption(idx, val)"
              @change="(val) => updateCaption(idx, val)"
            />
          </div>
          <el-button size="small" type="danger" @click="removePhoto(idx)">移除</el-button>
        </div>
      </div>
      <template #footer>
        <el-button @click="photoDialogVisible = false">关闭</el-button>
        <el-button type="primary" @click="savePhotoCaptions">保存说明</el-button>
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

// Photo management
const photoDialogVisible = ref(false)
const photoAlbum = ref(null)
const photoAlbumName = ref('')

const form = ref({ name: '', title: '', description: '', cover: '', story: '', photos: [] })

onMounted(reload)

async function reload() {
  loading.value = true
  try {
    const res = await fetch('/api/albums')
    albums.value = await res.json()
  } catch { albums.value = [] }
  finally { loading.value = false }
}

function onPhotoUploaded(res) {
  ElMessage.success('照片已上传')
  reload()
}

function showCreate() {
  editing.value = false
  form.value = { name: '', title: '', description: '', cover: '', story: '', photos: [] }
  dialogVisible.value = true
}

function editAlbum(album) {
  editing.value = true
  form.value = { ...album, photos: album.photos || [] }
  dialogVisible.value = true
}

function getPhotoUrl(photo) {
  return typeof photo === 'string' ? photo : (photo.url || '')
}
function getPhotoCaption(photo) {
  return typeof photo === 'string' ? '' : (photo.caption || '')
}

async function save() {
  if (!form.value.name || !form.value.title) {
    ElMessage.warning('请填写标识和标题')
    return
  }
  saving.value = true
  try {
    const payload = {
      ...form.value,
      // 没设封面就用第一张照片
      cover: form.value.cover || (form.value.photos?.length ? getPhotoUrl(form.value.photos[0]) : ''),
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

// Photo management
let originalPhotos = []

function editPhotos(album) {
  photoAlbum.value = JSON.parse(JSON.stringify(album))
  originalPhotos = photoAlbum.value.photos.map(p => getPhotoUrl(p))
  photoAlbumName.value = album.title
  photoDialogVisible.value = true
}

function updateCaption(idx, val) {
  if (!photoAlbum.value) return
  const photo = photoAlbum.value.photos[idx]
  if (typeof photo === 'string') {
    photoAlbum.value.photos[idx] = { url: photo, caption: val }
  } else {
    photo.caption = val
  }
}

function removePhoto(idx) {
  if (!photoAlbum.value) return
  photoAlbum.value.photos.splice(idx, 1)
}

async function savePhotoCaptions() {
  if (!photoAlbum.value) return
  try {
    const res = await fetch('/api/albums', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(photoAlbum.value),
    })
    if (res.ok) {
      // Delete removed photo files from disk
      const currentUrls = new Set(photoAlbum.value.photos.map(p => getPhotoUrl(p)))
      for (const url of originalPhotos) {
        if (url && !currentUrls.has(url)) {
          try {
            await fetch(`/api/media/${encodeURIComponent(url.replace(/^\//, ''))}`, { method: 'DELETE' })
          } catch { /* file may not exist */ }
        }
      }
      ElMessage.success('已保存')
      photoDialogVisible.value = false
      await reload()
    }
  } catch (e) {
    ElMessage.error('保存失败: ' + e.message)
  }
}
</script>
