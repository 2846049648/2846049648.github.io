<template>
  <div>
    <h2 class="text-2xl font-bold mb-6">个人资料设置</h2>
    <el-form label-width="100" class="max-w-xl">
      <el-form-item label="昵称">
        <el-input v-model="form.name" placeholder="你的昵称" />
      </el-form-item>
      <el-form-item label="头像 URL">
        <el-input v-model="form.avatar" placeholder="头像图片链接（可选）" />
      </el-form-item>
      <el-form-item label="个人简介">
        <el-input v-model="form.bio" type="textarea" :rows="3" placeholder="简单介绍一下自己" />
      </el-form-item>
      <el-form-item label="邮箱">
        <el-input v-model="form.email" placeholder="联系邮箱" />
      </el-form-item>
      <el-form-item label="社交链接">
        <div v-for="(val, key, idx) in form.social" :key="idx" class="flex items-center space-x-2 mb-2 w-full">
          <el-input v-model="form.social[key]" :placeholder="key + ' URL'" style="width: 200px" />
          <el-button @click="removeSocial(key)" type="danger" size="small">删除</el-button>
        </div>
        <div class="flex items-center space-x-2">
          <el-input v-model="newSocialKey" placeholder="平台名称" style="width: 120px" />
          <el-input v-model="newSocialUrl" placeholder="链接" style="width: 200px" />
          <el-button @click="addSocial" size="small">添加</el-button>
        </div>
      </el-form-item>
      <el-form-item>
        <el-button type="primary" :loading="saving" @click="save">保存</el-button>
      </el-form-item>
    </el-form>
  </div>
</template>

<script setup>
import { reactive, ref, onMounted } from 'vue'
import { ElMessage } from 'element-plus'

const form = reactive({
  name: '作者',
  avatar: '',
  bio: '这个人很懒，什么都没写…',
  email: '',
  social: {},
})
const saving = ref(false)
const newSocialKey = ref('')
const newSocialUrl = ref('')

onMounted(async () => {
  try {
    const res = await fetch('/api/profile')
    if (res.ok) Object.assign(form, await res.json())
  } catch {}
})

async function save() {
  saving.value = true
  try {
    const res = await fetch('/api/profile', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ ...form }),
    })
    if (res.ok) ElMessage.success('已保存')
    else ElMessage.error('保存失败')
  } catch (e) {
    ElMessage.error('保存失败: ' + e.message)
  } finally {
    saving.value = false
  }
}

function addSocial() {
  if (!newSocialKey.value || !newSocialUrl.value) return
  form.social[newSocialKey.value] = newSocialUrl.value
  newSocialKey.value = ''
  newSocialUrl.value = ''
}

function removeSocial(key) {
  delete form.social[key]
}
</script>
