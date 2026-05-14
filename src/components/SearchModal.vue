<template>
  <teleport to="body">
    <transition name="fade">
      <div
        v-if="visible"
        class="fixed inset-0 z-[100] flex items-start justify-center pt-[15vh]"
        @click.self="close"
      >
        <!-- Backdrop -->
        <div class="absolute inset-0 bg-black/50 backdrop-blur-sm" @click="close" />

        <!-- Modal -->
        <div
          class="relative w-full max-w-lg mx-4 rounded-2xl shadow-2xl border overflow-hidden"
          :style="{ background: 'var(--bg-surface)', borderColor: 'var(--border-color)' }"
        >
          <!-- Search input -->
          <div class="flex items-center px-4 border-b" :style="{ borderColor: 'var(--border-color)' }">
            <svg class="w-5 h-5 flex-shrink-0" :style="{ color: 'var(--text-light)' }" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0z"/>
            </svg>
            <input
              ref="inputRef"
              v-model="query"
              type="text"
              placeholder="搜索文章..."
              class="flex-1 px-3 py-4 text-base outline-none bg-transparent"
              :style="{ color: 'var(--text-primary)' }"
              @input="onSearch"
            />
            <kbd class="hidden sm:inline-flex text-xs px-1.5 py-0.5 rounded" :style="{ color: 'var(--text-light)', background: 'var(--bg-muted)' }">ESC</kbd>
          </div>

          <!-- Results -->
          <div v-if="query" class="max-h-80 overflow-y-auto">
            <div v-if="searching" class="text-center py-8 text-sm" :style="{ color: 'var(--text-light)' }">搜索中...</div>
            <div v-else-if="results.length === 0" class="text-center py-8 text-sm" :style="{ color: 'var(--text-light)' }">未找到相关文章</div>
            <router-link
              v-for="item in results" :key="item.slug"
              :to="`/posts/${item.slug}`"
              class="block px-4 py-3 transition-colors"
              :style="{ borderBottom: '1px solid var(--border-color)' }"
              @click="close"
            >
              <div class="flex items-center justify-between">
                <span class="font-medium text-sm" :style="{ color: 'var(--text-primary)' }">{{ item.title }}</span>
                <span class="text-xs flex-shrink-0 ml-2" :style="{ color: 'var(--text-light)' }">{{ item.date }}</span>
              </div>
              <div v-if="item.excerpt" class="text-xs mt-0.5 line-clamp-1" :style="{ color: 'var(--text-muted)' }">{{ item.excerpt }}</div>
              <div class="flex gap-1.5 mt-1">
                <span v-if="item.category" class="text-xs px-1.5 py-0.5 rounded-full" :style="{ background: 'var(--bg-muted)', color: 'var(--text-muted)' }">{{ item.category }}</span>
                <span v-for="tag in (item.tags || [])" :key="tag" class="text-xs px-1.5 py-0.5 rounded-full" style="background: #eff6ff; color: #2563eb;">{{ tag }}</span>
              </div>
            </router-link>
          </div>

          <!-- Empty state -->
          <div v-if="!query" class="px-4 py-6 text-center text-sm" :style="{ color: 'var(--text-light)' }">
            输入关键词搜索文章
          </div>
        </div>
      </div>
    </transition>
  </teleport>
</template>

<script setup>
import { ref, watch, nextTick } from 'vue'

const props = defineProps({ visible: Boolean })
const emit = defineEmits(['close'])

const query = ref('')
const results = ref([])
const searching = ref(false)
const inputRef = ref(null)
const postsIndex = ref([])

let index = null

async function ensureIndex() {
  if (index) return
  const FlexSearch = (await import('flexsearch')).default
  index = new FlexSearch.Document({
    document: {
      id: 'slug',
      index: ['title', 'excerpt', 'tags', 'category'],
      store: ['slug', 'title', 'excerpt', 'date', 'category', 'tags'],
    },
    tokenize: 'forward',
  })

  try {
    const url = import.meta.env.DEV ? '/api/posts' : '/posts/index.json'
    const res = await fetch(url)
    const posts = await res.json()
    posts.forEach(p => index.add(p))
    postsIndex.value = posts
  } catch {}
}

function onSearch() {
  if (!query.value.trim()) {
    results.value = []
    return
  }
  searching.value = true
  const q = query.value.trim()

  // FlexSearch gives document IDs, then we map back
  const raw = index.search(q, { limit: 10 })
  const ids = new Set()
  raw.forEach(field => field.result.forEach(id => ids.add(id)))
  results.value = [...ids].map(id => {
    const r = index.get(id, { store: ['slug', 'title', 'excerpt', 'date', 'category', 'tags'] })
    return r || postsIndex.value.find(p => p.slug === id) || { slug: id, title: id }
  }).filter(Boolean)

  searching.value = false
}

async function open() {
  query.value = ''
  results.value = []
  await ensureIndex()
  nextTick(() => inputRef.value?.focus())
}

watch(() => props.visible, (v) => { if (v) open() })

function close() {
  emit('close')
}
</script>
