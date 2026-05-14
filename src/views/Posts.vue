<template>
  <div>
    <h2 class="text-3xl font-bold mb-2" :style="{ color: 'var(--text-primary)' }">文章</h2>
    <p class="mb-6" :style="{ color: 'var(--text-muted)' }">分享技术、思考与生活</p>

    <!-- Search bar -->
    <div class="relative mb-6">
      <svg class="absolute left-3 top-1/2 -translate-y-1/2 w-4 h-4" :style="{ color: 'var(--text-light)' }" fill="none" stroke="currentColor" viewBox="0 0 24 24">
        <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0z"/>
      </svg>
      <input
        v-model="searchQuery"
        type="text"
        placeholder="搜索文章..."
        class="w-full pl-10 pr-4 py-2.5 rounded-xl border text-sm outline-none transition-all focus:ring-2 focus:ring-blue-500/30"
        :style="{ background: 'var(--bg-surface)', borderColor: 'var(--border-color)', color: 'var(--text-primary)' }"
      />
    </div>

    <!-- Filter chips: categories -->
    <div class="flex flex-wrap gap-2 mb-3">
      <button
        :class="activeCategory === '' ? 'filter-chip active' : 'filter-chip'"
        @click="activeCategory = ''"
      >全部</button>
      <button
        v-for="cat in categories" :key="cat"
        :class="activeCategory === cat ? 'filter-chip active' : 'filter-chip'"
        @click="activeCategory = cat"
      >{{ cat }}</button>
    </div>

    <!-- Filter chips: tags -->
    <div v-if="allTags.length" class="flex flex-wrap gap-1.5 mb-8">
      <span
        v-for="tag in allTags" :key="tag"
        :class="activeTag === tag ? 'tag-chip active' : 'tag-chip'"
        @click="activeTag = activeTag === tag ? '' : tag"
      >{{ tag }}</span>
    </div>

    <!-- Posts -->
    <div v-if="loading" class="text-center py-16" :style="{ color: 'var(--text-light)' }">
      <svg class="animate-spin w-8 h-8 mx-auto mb-3" fill="none" viewBox="0 0 24 24"><circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"/><path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4z"/></svg>
      加载中...
    </div>
    <div v-else-if="filteredPosts.length === 0" class="text-center py-16" :style="{ color: 'var(--text-light)' }">
      <svg class="w-12 h-12 mx-auto mb-3" :style="{ color: 'var(--text-light)' }" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="1.5" d="M19 20H5a2 2 0 01-2-2V6a2 2 0 012-2h10a2 2 0 012 2v1m2 13a2 2 0 01-2-2V7m2 13a2 2 0 002-2V9a2 2 0 00-2-2h-2m-4-3H9M7 16h6M7 8h6v4H7V8z"/></svg>
      暂无文章
    </div>
    <div v-else class="space-y-5">
      <article
        v-for="post in filteredPosts" :key="post.slug"
        class="rounded-xl p-6 shadow-sm hover:shadow-md transition-all border"
        :style="{ background: 'var(--bg-surface)', borderColor: 'var(--border-color)' }"
      >
        <router-link :to="`/posts/${post.slug}`" class="block">
          <div class="flex items-start space-x-4">
            <div class="w-10 h-10 rounded-full bg-gradient-to-br from-blue-500 to-purple-600 flex-shrink-0 flex items-center justify-center text-white text-sm font-bold shadow-sm">
              {{ (profile.name || '作').charAt(0).toUpperCase() }}
            </div>
            <div class="flex-1 min-w-0">
              <div class="flex items-center gap-2 mb-1">
                <span class="text-xs" :style="{ color: 'var(--text-muted)' }">{{ profile.name }}</span>
                <span class="text-gray-300">·</span>
                <span class="text-xs" :style="{ color: 'var(--text-muted)' }">{{ post.date }}</span>
                <span v-if="post.category" class="ml-auto category-badge">{{ post.category }}</span>
              </div>
              <h3 class="text-lg font-semibold transition-colors" :style="{ color: 'var(--text-primary)' }">
                {{ post.title }}
              </h3>
              <p v-if="post.excerpt" class="text-sm mt-1.5 line-clamp-2" :style="{ color: 'var(--text-muted)' }" v-html="highlightMatch(post.excerpt)"></p>
              <div class="flex flex-wrap gap-1.5 mt-3">
                <span v-for="tag in post.tags" :key="tag" class="tag-pill">{{ tag }}</span>
              </div>
            </div>
          </div>
        </router-link>
      </article>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import { useRoute } from 'vue-router'

const route = useRoute()
const posts = ref([])
const loading = ref(true)
const activeCategory = ref('')
const activeTag = ref('')
const searchQuery = ref('')
const profile = ref({ name: '作者' })

const categories = computed(() => {
  const set = new Set(posts.value.map(p => p.category).filter(Boolean))
  return [...set]
})

const allTags = computed(() => {
  const set = new Set()
  posts.value.forEach(p => (p.tags || []).forEach(t => set.add(t)))
  return [...set]
})

const filteredPosts = computed(() => {
  let result = posts.value
  if (activeCategory.value) {
    result = result.filter(p => p.category === activeCategory.value)
  }
  if (activeTag.value) {
    result = result.filter(p => p.tags && p.tags.includes(activeTag.value))
  }
  if (searchQuery.value.trim()) {
    const q = searchQuery.value.trim().toLowerCase()
    result = result.filter(p =>
      p.title.toLowerCase().includes(q) ||
      (p.excerpt && p.excerpt.toLowerCase().includes(q)) ||
      (p.tags && p.tags.some(t => t.toLowerCase().includes(q))) ||
      (p.category && p.category.toLowerCase().includes(q))
    )
  }
  return result
})

function highlightMatch(text) {
  if (!searchQuery.value.trim()) return text
  const q = searchQuery.value.trim()
  const idx = text.toLowerCase().indexOf(q.toLowerCase())
  if (idx === -1) return text
  return text.slice(0, idx) + '<span class="search-highlight">' + text.slice(idx, idx + q.length) + '</span>' + text.slice(idx + q.length)
}

onMounted(async () => {
  try {
    const dataUrl = import.meta.env.DEV ? '/api/posts' : '/posts/index.json'
    const [postsRes, profileRes] = await Promise.all([
      fetch(dataUrl),
      fetch(import.meta.env.DEV ? '/api/profile' : '/profile.json'),
    ])
    posts.value = await postsRes.json()
    if (profileRes.ok) profile.value = await profileRes.json()

    // Read ?tag= query param
    if (route.query.tag) {
      activeTag.value = route.query.tag
    }
  } catch {
    posts.value = []
  } finally {
    loading.value = false
  }
})
</script>

<style scoped>
.filter-chip {
  padding: 0.25rem 1rem;
  border-radius: 9999px;
  font-size: 0.875rem;
  cursor: pointer;
  transition: all 0.2s;
  border: 1px solid var(--border-color);
  color: var(--text-muted);
  background: transparent;
}
.filter-chip:hover {
  border-color: #93c5fd;
  color: #2563eb;
}
.filter-chip.active {
  background: #2563eb;
  color: white;
  border-color: #2563eb;
}
.filter-chip.active:hover {
  background: #1d4ed8;
}
.tag-chip {
  font-size: 0.75rem;
  padding: 0.125rem 0.625rem;
  border-radius: 9999px;
  background: var(--bg-muted);
  color: var(--text-muted);
  cursor: pointer;
  transition: all 0.2s;
  border: 1px solid transparent;
}
.tag-chip:hover {
  color: var(--text-primary);
}
.tag-chip.active {
  background: #eff6ff;
  color: #2563eb;
  border-color: #93c5fd;
}
.dark .tag-chip.active {
  background: rgba(37, 99, 235, 0.15);
  color: #60a5fa;
  border-color: rgba(96, 165, 250, 0.3);
}
.category-badge {
  font-size: 0.75rem;
  padding: 0.125rem 0.5rem;
  border-radius: 9999px;
  background: var(--bg-muted);
  color: var(--text-muted);
}
.tag-pill {
  font-size: 0.75rem;
  padding: 0.125rem 0.5rem;
  border-radius: 9999px;
  background: #eff6ff;
  color: #2563eb;
}
.dark .tag-pill {
  background: rgba(37, 99, 235, 0.15);
  color: #60a5fa;
}
</style>
