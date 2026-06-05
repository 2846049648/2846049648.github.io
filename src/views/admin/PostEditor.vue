<template>
  <div>
    <h2 class="text-2xl font-bold mb-6 flex items-center gap-2" style="color: #1e293b;">
      <span class="inline-block w-1.5 h-6 rounded-full" style="background: var(--color-primary);" />
      {{ isEdit ? '编辑文章' : '写新文章' }}
    </h2>
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
        <div class="border rounded-lg overflow-hidden w-full" style="border-color: #d1d5db;">
          <!-- Toolbar -->
          <div class="flex items-center flex-wrap gap-0.5 px-3 py-2 border-b select-none bg-white"
            style="border-color: #e5e7eb;">
            <!-- Headings -->
            <el-dropdown trigger="click" @command="insertHeading">
              <button class="md-toolbar-btn" title="标题">
                <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M6 4v16M18 4v16M6 12h12"/></svg>
              </button>
              <template #dropdown>
                <el-dropdown-menu>
                  <el-dropdown-item command="h1">H1 - 一级标题</el-dropdown-item>
                  <el-dropdown-item command="h2">H2 - 二级标题</el-dropdown-item>
                  <el-dropdown-item command="h3">H3 - 三级标题</el-dropdown-item>
                </el-dropdown-menu>
              </template>
            </el-dropdown>

            <div class="w-px h-5 mx-1 bg-gray-200" />

            <!-- Bold -->
            <button class="md-toolbar-btn" title="粗体" @click="wrapText('**', '**')">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M6 4h8a4 4 0 014 4 4 4 0 01-4 4H6zM6 12h9a4 4 0 014 4 4 4 0 01-4 4H6z"/></svg>
            </button>
            <!-- Italic -->
            <button class="md-toolbar-btn" title="斜体" @click="wrapText('*', '*')">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10 4h7M14 20H7M15 4L9 20"/></svg>
            </button>
            <!-- Inline Code -->
            <button class="md-toolbar-btn" title="行内代码" @click="wrapText('`', '`')">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M16 18l6-6-6-6M8 6l-6 6 6 6"/></svg>
            </button>
            <!-- Strikethrough -->
            <button class="md-toolbar-btn" title="删除线" @click="wrapText('~~', '~~')">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M6 12h12M4 6c0-1.1.9-2 2-2h12a2 2 0 012 2v2M4 18c0 1.1.9 2 2 2h12a2 2 0 002-2v-2"/></svg>
            </button>

            <div class="w-px h-5 mx-1 bg-gray-200" />

            <!-- Link -->
            <button class="md-toolbar-btn" title="插入链接" @click="insertLink">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10 13a5 5 0 007.54.54l3-3a5 5 0 00-7.07-7.07l-1.72 1.71M14 11a5 5 0 00-7.54-.54l-3 3a5 5 0 007.07 7.07l1.71-1.71"/></svg>
            </button>
            <!-- Image -->
            <button class="md-toolbar-btn" title="插入图片" @click="showImagePicker = true">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"/><circle cx="8.5" cy="8.5" r="1.5"/><path d="M21 15l-5-5L5 21"/></svg>
            </button>

            <div class="w-px h-5 mx-1 bg-gray-200" />

            <!-- Code block -->
            <button class="md-toolbar-btn" title="代码块" @click="insertCodeBlock">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M14 2H6a2 2 0 00-2 2v16a2 2 0 002 2h12a2 2 0 002-2V8z"/><polyline points="14 2 14 8 20 8"/><path d="M9 13l-2 2 2 2M15 13l2 2-2 2"/></svg>
            </button>
            <!-- Blockquote -->
            <button class="md-toolbar-btn" title="引用" @click="insertBlockquote">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M3 21c3 0 7-1 7-8V5c0-1.25-.756-2.017-2-2H4c-1.25 0-2 .75-2 1.972V11c0 1.25.75 2 2 2 1 0 1 0 1 1v1c0 1-1 2-2 2s-1 .008-1 1.031V20c0 1 0 1 1 1z"/><path d="M15 21c3 0 7-1 7-8V5c0-1.25-.757-2.017-2-2h-4c-1.25 0-2 .75-2 1.972V11c0 1.25.75 2 2 2h.75c0 2.25.25 4-2.75 4v3c0 1 0 1 1 1z"/></svg>
            </button>

            <div class="w-px h-5 mx-1 bg-gray-200" />

            <!-- Unordered List -->
            <button class="md-toolbar-btn" title="无序列表" @click="insertUnorderedList">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M8 6h13M8 12h13M8 18h13M3 6h.01M3 12h.01M3 18h.01"/></svg>
            </button>
            <!-- Ordered List -->
            <button class="md-toolbar-btn" title="有序列表" @click="insertOrderedList">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10 6h11M10 12h11M10 18h11M3 5v4M3 10v2M5 18H3l2.5-3.5A1.5 1.5 0 104 14"/></svg>
            </button>
            <!-- Horizontal Rule -->
            <button class="md-toolbar-btn" title="分割线" @click="insertHR">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M3 12h18"/></svg>
            </button>

            <div class="flex-1" />

            <!-- Tab switch -->
            <div class="flex items-center gap-1 text-xs">
              <button
                @click="activeTab = 'edit'"
                class="px-3 py-1.5 rounded-md transition-all font-medium"
                :style="{
                  background: activeTab === 'edit' ? '#e0f2fe' : 'transparent',
                  color: activeTab === 'edit' ? '#0284c7' : '#64748b'
                }"
              >编辑</button>
              <button
                @click="activeTab = 'preview'"
                class="px-3 py-1.5 rounded-md transition-all font-medium"
                :style="{
                  background: activeTab === 'preview' ? '#e0f2fe' : 'transparent',
                  color: activeTab === 'preview' ? '#0284c7' : '#64748b'
                }"
              >预览</button>
              <!-- Help button -->
              <button
                @click="showHelp = !showHelp"
                class="ml-1 w-7 h-7 rounded-full flex items-center justify-center transition-all text-xs font-bold"
                :style="{
                  background: showHelp ? '#dbeafe' : 'transparent',
                  color: showHelp ? '#2563eb' : '#94a3b8'
                }"
                title="Markdown 帮助"
              >?</button>
            </div>
          </div>

          <!-- Markdown Help panel -->
          <div v-if="showHelp" class="p-4 border-b text-sm space-y-2 bg-blue-50" style="border-color: #e5e7eb; color: #475569;">
            <p class="font-semibold" style="color: #2563eb;">Markdown 快速参考</p>
            <div class="grid grid-cols-2 gap-x-8 gap-y-1.5">
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;"># 标题</code> <span class="text-gray-400">→</span> 一级标题</div>
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;">## 标题</code> <span class="text-gray-400">→</span> 二级标题</div>
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;">**粗体**</code> <span class="text-gray-400">→</span> <strong>粗体</strong></div>
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;">*斜体*</code> <span class="text-gray-400">→</span> <em>斜体</em></div>
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;">`代码`</code> <span class="text-gray-400">→</span> 行内代码</div>
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;">```代码块```</code> <span class="text-gray-400">→</span> 代码块</div>
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;">[文本](url)</code> <span class="text-gray-400">→</span> 链接</div>
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;">![alt](url)</code> <span class="text-gray-400">→</span> 图片</div>
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;">- 列表</code> <span class="text-gray-400">→</span> 无序列表</div>
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;">1. 列表</code> <span class="text-gray-400">→</span> 有序列表</div>
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;">&gt; 引用</code> <span class="text-gray-400">→</span> 引用块</div>
              <div><code class="bg-blue-100 px-1 rounded" style="color: #2563eb;">---</code> <span class="text-gray-400">→</span> 分割线</div>
            </div>
          </div>

          <!-- Editor / Preview -->
          <textarea
            v-if="activeTab === 'edit'"
            ref="textareaRef"
            v-model="content"
            class="w-full font-mono border-0 resize-y outline-none p-4 leading-relaxed"
            :rows="24"
            placeholder="使用 Markdown 编写文章... 点工具栏按钮快速插入格式"
            style="background: #fafafa; color: #1e293b; min-height: 540px; font-size: 0.95rem; tab-size: 2;"
          />
          <div v-else class="p-6 prose max-w-none min-h-[540px]" style="background: #fafafa;" v-html="previewHtml" />
        </div>
      </el-form-item>
      <el-form-item>
        <el-button type="primary" :loading="saving" @click="save">{{ isEdit ? '更新' : '保存' }}</el-button>
        <el-button @click="cancel">取消</el-button>
      </el-form-item>
    </el-form>

    <!-- Image Picker Dialog -->
    <el-dialog v-model="showImagePicker" title="选择图片" width="640">
      <div v-if="images.length === 0" class="text-center py-8" style="color: #94a3b8;">
        暂无图片，请先在媒体库上传
      </div>
      <div v-else class="grid grid-cols-3 gap-3">
        <div
          v-for="img in images" :key="img.name"
          class="group relative rounded-lg overflow-hidden border-2 cursor-pointer transition-all"
          :class="selectedImage === img.url ? 'ring-2 ring-blue-500 border-blue-500' : 'border-gray-200 hover:border-gray-300'"
          @click="selectImage(img)"
        >
          <img :src="img.url" class="w-full h-24 object-cover" />
          <div class="absolute inset-0 bg-black/0 group-hover:bg-black/20 transition-colors" />
          <div v-if="selectedImage === img.url" class="absolute top-1 right-1 w-5 h-5 rounded-full bg-blue-500 text-white flex items-center justify-center text-xs font-bold">✓</div>
        </div>
      </div>
      <template #footer>
        <el-button @click="showImagePicker = false">取消</el-button>
        <el-button type="primary" :disabled="!selectedImage" @click="insertSelectedImage">插入图片</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, nextTick, watch } from 'vue'
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
const textareaRef = ref(null)
const showHelp = ref(false)

// Image picker
const showImagePicker = ref(false)
const images = ref([])
const selectedImage = ref('')

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

// ─── Toolbar Actions ───

function getTextarea() {
  return textareaRef.value
}

function insertAtCursor(text) {
  const ta = getTextarea()
  if (!ta) return
  const start = ta.selectionStart
  const end = ta.selectionEnd
  const before = content.value.substring(0, start)
  const after = content.value.substring(end)
  content.value = before + text + after
  nextTick(() => {
    ta.focus()
    ta.setSelectionRange(start + text.length, start + text.length)
  })
}

function wrapText(before, after) {
  const ta = getTextarea()
  if (!ta) return
  const start = ta.selectionStart
  const end = ta.selectionEnd
  const selected = content.value.substring(start, end)
  const newText = before + (selected || 'text') + after
  const fullBefore = content.value.substring(0, start)
  const fullAfter = content.value.substring(end)
  content.value = fullBefore + newText + fullAfter
  nextTick(() => {
    ta.focus()
    if (selected) {
      ta.setSelectionRange(start + before.length, start + before.length + selected.length)
    } else {
      ta.setSelectionRange(start + before.length, start + before.length + newText.length - after.length)
    }
  })
}

function insertHeading(cmd) {
  const ta = getTextarea()
  if (!ta) return
  const start = ta.selectionStart
  const lineStart = content.value.lastIndexOf('\n', start - 1) + 1
  const prefix = cmd === 'h1' ? '# ' : cmd === 'h2' ? '## ' : '### '
  const before = content.value.substring(0, lineStart)
  const line = content.value.substring(lineStart)
  const after = content.value.substring(start)
  const existingMatch = line.match(/^(#{1,6})\s/)
  if (existingMatch) {
    content.value = before + prefix + line.substring(existingMatch[0].length)
  } else {
    content.value = before + prefix + line
  }
  ta.focus()
}

function insertLink() {
  const ta = getTextarea()
  if (!ta) return
  const start = ta.selectionStart
  const end = ta.selectionEnd
  const selected = content.value.substring(start, end)
  const text = selected || '链接文本'
  const linkText = `[${text}](url)`
  const before = content.value.substring(0, start)
  const afterStr = content.value.substring(end)
  content.value = before + linkText + afterStr
  nextTick(() => {
    ta.focus()
    if (!selected) {
      ta.setSelectionRange(start + 1, start + 5)
    } else {
      ta.setSelectionRange(start + text.length + 2, start + text.length + 5)
    }
  })
}

function insertCodeBlock() {
  insertAtCursor('\n```\n\n```\n')
}

function insertBlockquote() {
  const ta = getTextarea()
  if (!ta) return
  const start = ta.selectionStart
  const lineStart = content.value.lastIndexOf('\n', start - 1) + 1
  const before = content.value.substring(0, lineStart)
  const after = content.value.substring(start)
  content.value = before + '> ' + after
  ta.focus()
}

function insertUnorderedList() {
  const ta = getTextarea()
  if (!ta) return
  const start = ta.selectionStart
  const lineStart = content.value.lastIndexOf('\n', start - 1) + 1
  const before = content.value.substring(0, lineStart)
  const after = content.value.substring(start)
  content.value = before + '- ' + after
  ta.focus()
}

function insertOrderedList() {
  const ta = getTextarea()
  if (!ta) return
  const start = ta.selectionStart
  const lineStart = content.value.lastIndexOf('\n', start - 1) + 1
  const before = content.value.substring(0, lineStart)
  const after = content.value.substring(start)
  content.value = before + '1. ' + after
  ta.focus()
}

function insertHR() {
  const ta = getTextarea()
  if (!ta) return
  const start = ta.selectionStart
  const before = content.value.substring(0, start)
  const after = content.value.substring(start)
  const needsNewline = before.length > 0 && !before.endsWith('\n')
  content.value = before + (needsNewline ? '\n' : '') + '---\n' + after
  ta.focus()
}

// ─── Image Picker ───

function selectImage(img) {
  selectedImage.value = selectedImage.value === img.url ? '' : img.url
}

function insertSelectedImage() {
  if (!selectedImage.value) return
  const alt = selectedImage.value.split('/').pop().replace(/\.[^/.]+$/, '')
  insertAtCursor(`![${alt}](${selectedImage.value})`)
  showImagePicker.value = false
  selectedImage.value = ''
}

// Load images when dialog opens
watch(showImagePicker, async (val) => {
  if (val) {
    try {
      const res = await fetch('/api/media')
      images.value = await res.json()
    } catch {
      images.value = []
    }
    selectedImage.value = ''
  }
})

// ─── Save ───

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

<style scoped>
.md-toolbar-btn {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  width: 32px;
  height: 32px;
  border-radius: 6px;
  border: none;
  background: transparent;
  color: #64748b;
  cursor: pointer;
  transition: all 0.15s;
}
.md-toolbar-btn:hover {
  background: #e0f2fe;
  color: #0284c7;
}
.md-toolbar-btn:active {
  transform: scale(0.95);
}
textarea::placeholder {
  color: #94a3b8;
}
</style>
