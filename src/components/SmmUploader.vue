<template>
  <div>
    <el-button
      type="warning"
      :loading="uploading"
      :disabled="!token"
      @click="selectFile"
    >
      <svg class="w-4 h-4 mr-1.5 inline" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 16l4.586-4.586a2 2 0 012.828 0L16 16m-2-2l1.586-1.586a2 2 0 012.828 0L20 14m-6-6h.01M6 20h12a2 2 0 002-2V6a2 2 0 00-2-2H6a2 2 0 00-2 2v12a2 2 0 002 2z"/>
      </svg>
      上传到 sm.ms
    </el-button>

    <input
      ref="fileInput"
      type="file"
      accept="image/*"
      class="hidden"
      @change="upload"
    />

    <el-dialog v-model="resultVisible" title="上传成功" width="480">
      <div class="space-y-3">
        <img :src="resultUrl" class="w-full rounded-lg max-h-64 object-contain bg-gray-100" />
        <div>
          <label class="text-xs text-gray-500 mb-1 block">图片 URL</label>
          <div class="flex gap-2">
            <el-input :model-value="resultUrl" readonly />
            <el-button @click="copy(resultUrl)">复制</el-button>
          </div>
        </div>
        <div>
          <label class="text-xs text-gray-500 mb-1 block">Markdown</label>
          <div class="flex gap-2">
            <el-input :model-value="`![image](${resultUrl})`" readonly />
            <el-button @click="copy(`![image](${resultUrl})`)">复制</el-button>
          </div>
        </div>
      </div>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, computed } from 'vue'
import { ElMessage } from 'element-plus'

const props = defineProps({
  token: { type: String, default: '' },
})

const emit = defineEmits(['uploaded'])

const fileInput = ref(null)
const uploading = ref(false)
const resultVisible = ref(false)
const resultUrl = ref('')

function selectFile() {
  fileInput.value?.click()
}

async function upload(e) {
  const file = e.target.files?.[0]
  if (!file) return

  uploading.value = true
  try {
    const formData = new FormData()
    formData.append('smfile', file)

    const res = await fetch('https://sm.ms/api/v2/upload', {
      method: 'POST',
      headers: {
        Authorization: props.token,
      },
      body: formData,
    })

    const data = await res.json()
    if (data.code === 'success') {
      resultUrl.value = data.data.url
      resultVisible.value = true
      emit('uploaded', data.data)
      ElMessage.success('上传成功')
    } else {
      ElMessage.error(data.message || '上传失败')
    }
  } catch (err) {
    ElMessage.error('上传失败: ' + err.message)
  } finally {
    uploading.value = false
    e.target.value = ''
  }
}

async function copy(text) {
  try {
    await navigator.clipboard.writeText(text)
    ElMessage.success('已复制')
  } catch {
    ElMessage.warning('复制失败，请手动复制')
  }
}
</script>
