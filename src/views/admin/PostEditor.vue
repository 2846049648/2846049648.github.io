<template>
  <div>
    <div class="flex items-center justify-between mb-6">
      <h2 class="text-2xl font-bold flex items-center gap-2" style="color: #1e293b;">
        <span class="inline-block w-1.5 h-6 rounded-full" style="background: var(--color-primary);" />
        {{ isEdit ? '编辑文章' : '写新文章' }}
      </h2>
      <el-button size="small" @click="importMd" title="选择本地 Markdown 文件，自动填入标题与正文">
        <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"/><polyline points="17 8 12 3 7 8"/><line x1="12" y1="3" x2="12" y2="15"/></svg>
        <span class="ml-1">导入 Markdown</span>
      </el-button>
    </div>
    <input
      ref="fileInputRef"
      type="file"
      accept=".md,.markdown,text/markdown,text/plain"
      style="display: none;"
      @change="onImportFile"
    />
    <el-form label-width="80" @submit.prevent>
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
        <div class="border rounded-lg w-full" style="border-color: #d1d5db; background: #fff;">
          <!-- Toolbar (sticky: 内容写长滚动后仍可点加粗等格式按钮) -->
          <div
            class="sticky top-3 z-20 flex items-center flex-wrap gap-0.5 px-3 py-2 border-b select-none bg-white rounded-t-lg"
            style="border-color: #e5e7eb; box-shadow: 0 6px 16px -10px rgba(15, 23, 42, 0.18);"
          >
            <!-- Headings -->
            <el-dropdown trigger="click" @command="insertHeading">
              <button type="button" class="md-toolbar-btn" title="标题">
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
            <button type="button" class="md-toolbar-btn" title="粗体" @click="wrapText('**', '**')">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M6 4h8a4 4 0 014 4 4 4 0 01-4 4H6zM6 12h9a4 4 0 014 4 4 4 0 01-4 4H6z"/></svg>
            </button>
            <!-- Italic -->
            <button type="button" class="md-toolbar-btn" title="斜体" @click="wrapText('*', '*')">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10 4h7M14 20H7M15 4L9 20"/></svg>
            </button>
            <!-- Inline Code -->
            <button type="button" class="md-toolbar-btn" title="行内代码" @click="wrapText('`', '`')">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M16 18l6-6-6-6M8 6l-6 6 6 6"/></svg>
            </button>
            <!-- Strikethrough -->
            <button type="button" class="md-toolbar-btn" title="删除线" @click="wrapText('~~', '~~')">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M6 12h12M4 6c0-1.1.9-2 2-2h12a2 2 0 012 2v2M4 18c0 1.1.9 2 2 2h12a2 2 0 002-2v-2"/></svg>
            </button>

            <div class="w-px h-5 mx-1 bg-gray-200" />

            <!-- Link -->
            <button type="button" class="md-toolbar-btn" title="插入链接" @click="insertLink">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10 13a5 5 0 007.54.54l3-3a5 5 0 00-7.07-7.07l-1.72 1.71M14 11a5 5 0 00-7.54-.54l-3 3a5 5 0 007.07 7.07l1.71-1.71"/></svg>
            </button>
            <!-- Image -->
            <button type="button" class="md-toolbar-btn" title="插入图片（或直接在编辑框 Ctrl+V 粘贴）" @click="showImagePicker = true">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"/><circle cx="8.5" cy="8.5" r="1.5"/><path d="M21 15l-5-5L5 21"/></svg>
            </button>

            <div class="w-px h-5 mx-1 bg-gray-200" />

            <!-- Code block -->
            <button type="button" class="md-toolbar-btn" title="代码块" @click="insertCodeBlock">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M14 2H6a2 2 0 00-2 2v16a2 2 0 002 2h12a2 2 0 002-2V8z"/><polyline points="14 2 14 8 20 8"/><path d="M9 13l-2 2 2 2M15 13l2 2-2 2"/></svg>
            </button>
            <!-- Blockquote -->
            <button type="button" class="md-toolbar-btn" title="引用" @click="insertBlockquote">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M3 21c3 0 7-1 7-8V5c0-1.25-.756-2.017-2-2H4c-1.25 0-2 .75-2 1.972V11c0 1.25.75 2 2 2 1 0 1 0 1 1v1c0 1-1 2-2 2s-1 .008-1 1.031V20c0 1 0 1 1 1z"/><path d="M15 21c3 0 7-1 7-8V5c0-1.25-.757-2.017-2-2h-4c-1.25 0-2 .75-2 1.972V11c0 1.25.75 2 2 2h.75c0 2.25.25 4-2.75 4v3c0 1 0 1 1 1z"/></svg>
            </button>

            <div class="w-px h-5 mx-1 bg-gray-200" />

            <!-- Unordered List -->
            <button type="button" class="md-toolbar-btn" title="无序列表" @click="insertUnorderedList">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M8 6h13M8 12h13M8 18h13M3 6h.01M3 12h.01M3 18h.01"/></svg>
            </button>
            <!-- Ordered List -->
            <button type="button" class="md-toolbar-btn" title="有序列表" @click="insertOrderedList">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10 6h11M10 12h11M10 18h11M3 5v4M3 10v2M5 18H3l2.5-3.5A1.5 1.5 0 104 14"/></svg>
            </button>
            <!-- Horizontal Rule -->
            <button type="button" class="md-toolbar-btn" title="分割线" @click="insertHR">
              <svg class="w-4 h-4" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M3 12h18"/></svg>
            </button>

            <div class="flex-1" />

            <!-- 分屏 / 编辑 / 预览 -->
            <div class="flex items-center gap-1 text-xs">
              <button
                type="button"
                title="左右双栏，实时预览"
                @click="activeTab = 'split'"
                class="px-3 py-1.5 rounded-md transition-all font-medium"
                :style="{
                  background: activeTab === 'split' ? '#e0f2fe' : 'transparent',
                  color: activeTab === 'split' ? '#0284c7' : '#64748b'
                }"
              >分屏</button>
              <button
                type="button"
                title="只显示编辑框"
                @click="activeTab = 'edit'"
                class="px-3 py-1.5 rounded-md transition-all font-medium"
                :style="{
                  background: activeTab === 'edit' ? '#e0f2fe' : 'transparent',
                  color: activeTab === 'edit' ? '#0284c7' : '#64748b'
                }"
              >编辑</button>
              <button
                type="button"
                title="只显示预览效果"
                @click="activeTab = 'preview'"
                class="px-3 py-1.5 rounded-md transition-all font-medium"
                :style="{
                  background: activeTab === 'preview' ? '#e0f2fe' : 'transparent',
                  color: activeTab === 'preview' ? '#0284c7' : '#64748b'
                }"
              >预览</button>
              <!-- Help button -->
              <button
                type="button"
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
            <p class="text-xs" style="color: #64748b;">提示：普通回车即换行；空一行才是另起一段（段落间距更大）。</p>
          </div>

          <!-- Editor / Live Preview -->
          <div class="editor-panes overflow-hidden rounded-b-lg" :class="{ split: activeTab === 'split' }">
            <textarea
              v-if="activeTab !== 'preview'"
              ref="textareaRef"
              v-model="content"
              @keydown="onContentKeydown"
              @paste="onPasteImage"
              class="w-full font-mono border-0 resize-y outline-none p-4 leading-relaxed"
              :rows="24"
              placeholder="使用 Markdown 编写文章... 点工具栏按钮快速插入格式"
              style="background: #fafafa; color: #1e293b; min-height: 540px; font-size: 0.95rem; tab-size: 2;"
            />
            <div
              v-if="activeTab !== 'edit'"
              class="preview-pane p-4 prose max-w-none min-h-[540px] overflow-auto"
              style="background: #fafafa;"
              v-html="previewHtml"
            />
          </div>
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
        媒体库暂无图片<br />
        <span class="text-xs">可以直接在编辑框里 Ctrl+V 粘贴截图/图片，会自动上传到这里</span>
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
import { useRoute, useRouter, onBeforeRouteLeave } from 'vue-router'
import { ElMessage, ElMessageBox } from 'element-plus'
import { marked } from '../../lib/markdown.js'

const route = useRoute()
const router = useRouter()
const isEdit = computed(() => !!route.params.slug)

const slug = ref('')
const title = ref('')
const category = ref('')
const tags = ref([])
const excerpt = ref('')
const content = ref('')
const activeTab = ref('split') // 'split' = 双栏实时预览 | 'edit' | 'preview'
const saving = ref(false)
const textareaRef = ref(null)
const fileInputRef = ref(null)
const showHelp = ref(false)

// Image picker
const showImagePicker = ref(false)
const images = ref([])
const selectedImage = ref('')
const pastingImage = ref(false) // 粘贴图片上传中

const previewHtml = computed(() => {
  try {
    // breaks: 单次回车即换行（与正文发布后的渲染保持一致）
    return marked(content.value || '', { breaks: true, gfm: true })
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

// ─── 列表：跨行编号 / 续号 / 回车自动续列表 ───

function getLineInfo(text) {
  const lines = text.split('\n')
  const starts = []
  let off = 0
  for (let i = 0; i < lines.length; i++) {
    starts.push(off)
    off += lines[i].length + 1
  }
  return { lines, starts }
}

function lineAt(starts, pos) {
  let lo = 0, hi = starts.length - 1, res = 0
  while (lo <= hi) {
    const mid = (lo + hi) >> 1
    if (starts[mid] <= pos) { res = mid; lo = mid + 1 } else { hi = mid - 1 }
  }
  return res
}

// 光标位置之前是否处于 ``` 围栏代码块内
function insideFence(text, pos) {
  const pre = text.slice(0, pos)
  const fenceLines = pre.match(/^```.*$/gm)
  return fenceLines ? fenceLines.length % 2 === 1 : false
}

// 把光标所在行（或多行选中内容）转成列表。
// numbered=true  → 有序列表，自动 1.2.3.…
// numbered=false → 无序列表（-）
function applyList(numbered) {
  const ta = getTextarea()
  if (!ta) return
  const start = ta.selectionStart
  const end = ta.selectionEnd
  const { lines, starts } = getLineInfo(content.value)
  const first = lineAt(starts, start)
  const last = end > start ? lineAt(starts, end - 1) : first
  const singleEmpty = first === last && lines[first].trim() === ''

  // 在已有有序列表的下一空行点击时，接续上一个序号
  let startNum = 1
  if (singleEmpty && numbered) {
    for (let i = first - 1; i >= 0; i--) {
      if (lines[i].trim() === '') continue
      const m = lines[i].match(/^\s*(\d+)\s*[.)]/)
      if (m) startNum = parseInt(m[1], 10) + 1
      break // 只看紧邻的最近一行
    }
  }

  if (singleEmpty) {
    // 单个空行：直接插入标记（有序则接续上一个序号）
    lines[first] = numbered ? `${startNum}. ` : '- '
  } else {
    let counter = 1
    for (let i = first; i <= last; i++) {
      const line = lines[i]
      if (line.trim() === '') continue
      const indent = (line.match(/^\s*/) || [''])[0]
      // 先去掉行首已有的列表标记，再按需加数字/破折号（有序/无序可互相转换）
      const body = line.trim().replace(/^(?:\d+\s*[.)]\s+|[-*+]\s+)/, '')
      lines[i] = indent + (numbered ? `${counter}. ` : '- ') + body
      if (numbered) counter++
    }
  }

  content.value = lines.join('\n')

  const after = getLineInfo(content.value)
  const lineStartPos = after.starts[first]
  nextTick(() => {
    ta.focus()
    if (singleEmpty) {
      // 光标放到刚插入的标记后面，方便直接输入
      ta.setSelectionRange(lineStartPos + lines[first].length, lineStartPos + lines[first].length)
    } else {
      const lineEndPos = after.starts[last] + lines[last].length
      ta.setSelectionRange(lineEndPos, lineEndPos)
    }
  })
}

function insertUnorderedList() {
  applyList(false)
}

function insertOrderedList() {
  applyList(true)
}

// 在行尾按回车：有序列表自动续下一个序号（1.→2.），无序列表续“- ”；
// 在空的列表项上按回车：取消标记、退出列表。代码块与缩进代码内不处理。
function onContentKeydown(e) {
  if (e.key !== 'Enter' || e.shiftKey || e.ctrlKey || e.metaKey || e.altKey) return
  const ta = getTextarea()
  if (!ta) return
  const start = ta.selectionStart
  const end = ta.selectionEnd
  if (start !== end) return
  const text = content.value
  if (insideFence(text, start)) return
  const { lines, starts } = getLineInfo(text)
  const li = lineAt(starts, start)
  const line = lines[li]
  if (/^\s{4}/.test(line)) return // 缩进代码块不续行
  const lineStartPos = starts[li]
  const lineEndPos = lineStartPos + line.length
  if (start < lineEndPos) return // 仅在行尾回车时触发
  const m = line.match(/^(\s*)(?:(?:(\d+)\s*([.)]))|([-*+]))(\s+)(.*)$/)
  if (!m) return
  const indent = m[1]
  const num = m[2]
  const sep = m[3]
  const bullet = m[4]
  const body = m[6]
  e.preventDefault()
  if (body.trim() === '') {
    // 空列表项 → 去掉本行标记，退出列表
    lines[li] = ''
    content.value = lines.join('\n')
    const caret = lineStartPos
    nextTick(() => { ta.focus(); ta.setSelectionRange(caret, caret) })
    return
  }
  const nextMarker = num ? `${indent}${parseInt(num, 10) + 1}${sep} ` : `${indent}${bullet} `
  content.value = text.slice(0, start) + '\n' + nextMarker + text.slice(start)
  const caret = start + 1 + nextMarker.length
  nextTick(() => { ta.focus(); ta.setSelectionRange(caret, caret) })
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

// ─── 粘贴图片：复制粘贴即上传媒体库并插入 ───

// 从剪贴板事件里取出图片文件（截图 / 复制图片都能取到）
function getClipboardImages(e) {
  const files = []
  const cd = e.clipboardData
  if (!cd) return files
  if (cd.files) {
    for (const f of cd.files) {
      if (f.type && f.type.startsWith('image/')) files.push(f)
    }
  }
  // 某些浏览器（如部分网站复制的图片）只以 clipboard item 提供
  if (!files.length && cd.items) {
    for (const item of cd.items) {
      if (item.kind === 'file' && item.type.startsWith('image/')) {
        const f = item.getAsFile()
        if (f) files.push(f)
      }
    }
  }
  return files
}

async function uploadImage(file) {
  const fd = new FormData()
  // 剪贴板粘贴的文件常没有名字/扩展名，补一个以便后端识别类型
  const name = file.name && file.name.trim()
    ? file.name
    : `pasted-${Date.now()}.${(file.type.split('/')[1] || 'png').replace('jpeg', 'jpg')}`
  fd.append('file', file, name)
  // source=paste：后端存成 paste-* 临时图，保存/离开时对账清理
  const res = await fetch('/api/upload/image?source=paste', { method: 'POST', body: fd })
  const data = await res.json()
  if (!data.url) throw new Error(data.error || '上传失败')
  return data.url
}

async function onPasteImage(e) {
  const files = getClipboardImages(e)
  if (!files.length) return // 粘贴的是文字等，交给默认行为
  e.preventDefault()
  if (pastingImage.value) { ElMessage.info('图片正在上传，请稍候'); return }
  pastingImage.value = true
  try {
    const ta = getTextarea()
    if (!ta) return
    let pos = ta.selectionStart
    for (const file of files) {
      const url = await uploadImage(file)
      const before = content.value.slice(0, pos)
      const after = content.value.slice(pos)
      const needBreak = pos > 0 && content.value[pos - 1] !== '\n'
      // alt 取自文件名（去掉扩展名），并清掉会破坏 Markdown 的字符
      const alt = (file.name || 'image').replace(/\.[^.]+$/, '').replace(/[\]\n]/g, '')
      const md = (needBreak ? '\n' : '') + `![${alt || '图片'}](${url})` + '\n'
      content.value = before + md + after
      pos = before.length + md.length // 下一张插到本张之后
    }
    nextTick(() => {
      ta.focus()
      ta.setSelectionRange(pos, pos)
    })
    ElMessage.success('图片已上传并插入')
  } catch (err) {
    ElMessage.error('图片上传失败: ' + ((err && err.message) || err))
  } finally {
    pastingImage.value = false
  }
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

// ─── 导入 Markdown 文件 ───

// 与 scripts/generate-static-data.js 保持一致的前置信息解析
function parseImportFrontmatter(text) {
  const match = text.match(/^---\n([\s\S]*?)\n---\n([\s\S]*)$/)
  if (!match) return { data: {}, content: text }
  const data = {}
  match[1].split('\n').forEach((line) => {
    const idx = line.indexOf(':')
    if (idx === -1) return
    const key = line.slice(0, idx).trim()
    const val = line.slice(idx + 1).trim()
    if (val.startsWith('[') && val.endsWith(']')) {
      data[key] = val.slice(1, -1).split(',').map((s) => s.trim().replace(/['"]/g, ''))
    } else {
      data[key] = val.replace(/^['"]|['"]$/g, '')
    }
  })
  return { data, content: match[2] }
}

function slugFromFileName(name) {
  const base = name.replace(/\.(md|markdown)$/i, '').trim()
  const slug = base.toLowerCase()
    .replace(/\s+/g, '-')
    .replace(/[^\p{L}\p{N}-]/gu, '')
    .replace(/-{2,}/g, '-')
    .replace(/^-|-$/g, '')
  return slug || 'imported-post'
}

function hasUnsavedContent() {
  return !!(title.value.trim() || slug.value.trim() || content.value.trim()
    || excerpt.value.trim() || category.value.trim() || tags.value.length)
}

function importMd() {
  const input = fileInputRef.value
  if (input) input.click()
}

function resetFileInput(e) {
  if (e && e.target) e.target.value = ''
}

async function onImportFile(e) {
  const file = e.target.files && e.target.files[0]
  if (!file) return
  try {
    const text = await file.text()
    const { data, content: body } = parseImportFrontmatter(text)

    // 已有内容时先确认，避免误覆盖
    if (hasUnsavedContent()) {
      try {
        await ElMessageBox.confirm(
          '导入会覆盖当前已填写的内容，确定继续吗？',
          '导入 Markdown',
          { confirmButtonText: '覆盖并导入', cancelButtonText: '取消', type: 'warning' }
        )
      } catch {
        resetFileInput(e)
        return
      }
    }

    content.value = body
    const baseName = file.name.replace(/\.(md|markdown)$/i, '')
    title.value = data.title ? String(data.title) : baseName
    if (!isEdit.value) {
      const fmSlug = data.slug ? String(data.slug).trim() : ''
      slug.value = fmSlug || slugFromFileName(file.name)
    }
    if (data.category) category.value = String(data.category)
    if (data.tags) tags.value = Array.isArray(data.tags) ? data.tags : [String(data.tags)]
    if (data.excerpt) excerpt.value = String(data.excerpt)
    activeTab.value = 'split'
    ElMessage.success(`已导入「${title.value}」`)
  } catch (err) {
    ElMessage.error('导入失败：' + ((err && err.message) || err))
  } finally {
    resetFileInput(e)
  }
}

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

// 离开编辑器（取消 / 菜单跳转 / 浏览器后退）时，清理本次草稿里粘贴过但
// 没被任何已保存文章引用的临时图；保存成功的引用图不受影响。
onBeforeRouteLeave(() => {
  try {
    fetch('/api/media/reconcile', { method: 'POST' }).catch(() => {})
  } catch { /* ignore */ }
})
</script>

<style scoped>
/* 双栏（分屏）实时预览 */
.editor-panes {
  display: grid;
  grid-template-columns: minmax(0, 1fr);
}
.editor-panes.split {
  grid-template-columns: minmax(0, 1fr) minmax(0, 1fr);
}
.editor-panes .preview-pane {
  min-width: 0;
}
.editor-panes.split .preview-pane {
  border-left: 1px solid #e5e7eb;
}
/* 窄屏自动退回单栏（预览移到下方） */
@media (max-width: 960px) {
  .editor-panes.split {
    grid-template-columns: minmax(0, 1fr);
  }
  .editor-panes.split .preview-pane {
    border-left: none;
    border-top: 1px solid #e5e7eb;
  }
}
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
