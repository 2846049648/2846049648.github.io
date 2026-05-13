<template>
  <div>
    <h2 class="text-3xl font-bold text-gray-900 mb-2">文章</h2>
    <p class="text-gray-500 mb-6">分享技术、思考与生活</p>

    <!-- Filter chips -->
    <div class="flex flex-wrap gap-2 mb-8">
      <button
        :class="filter === '' ? 'filter-chip active' : 'filter-chip'"
        @click="filter = ''"
      >全部</button>
      <button
        v-for="cat in categories" :key="cat"
        :class="filter === cat ? 'filter-chip active' : 'filter-chip'"
        @click="filter = cat"
      >{{ cat }}</button>
    </div>

    <!-- Posts -->
    <div v-if="loading" class="text-center py-16 text-gray-400">
      <svg class="animate-spin w-8 h-8 mx-auto mb-3" fill="none" viewBox="0 0 24 24"><circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"/><path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4z"/></svg>
      加载中...
    </div>
    <div v-else-if="filteredPosts.length === 0" class="text-center py-16 text-gray-400">
      <svg class="w-12 h-12 mx-auto mb-3 text-gray-300" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="1.5" d="M19 20H5a2 2 0 01-2-2V6a2 2 0 012-2h10a2 2 0 012 2v1m2 13a2 2 0 01-2-2V7m2 13a2 2 0 002-2V9a2 2 0 00-2-2h-2m-4-3H9M7 16h6M7 8h6v4H7V8z"/></svg>
      暂无文章
    </div>
    <div v-else class="space-y-5">
      <article
        v-for="post in filteredPosts" :key="post.slug"
        class="group bg-white rounded-xl p-6 shadow-sm hover:shadow-md transition-all border border-gray-100"
      >
        <router-link :to="`/posts/${post.slug}`" class="block">
          <div class="flex items-start space-x-4">
            <!-- Author avatar -->
            <div class="w-10 h-10 rounded-full bg-gradient-to-br from-blue-500 to-purple-600 flex-shrink-0 flex items-center justify-center text-white text-sm font-bold shadow-sm">
              {{ (profile.name || '作').charAt(0).toUpperCase() }}
            </div>
            <div class="flex-1 min-w-0">
              <div class="flex items-center gap-2 mb-1">
                <span class="text-xs text-gray-400">{{ profile.name }}</span>
                <span class="text-gray-300">·</span>
                <span class="text-xs text-gray-400">{{ post.date }}</span>
                <span v-if="post.category" class="ml-auto category-badge">{{ post.category }}</span>
              </div>
              <h3 class="text-lg font-semibold text-gray-900 group-hover:text-blue-600 transition-colors">
                {{ post.title }}
              </h3>
              <p v-if="post.excerpt" class="text-sm text-gray-500 mt-1.5 line-clamp-2">{{ post.excerpt }}</p>
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

const posts = ref([])
const loading = ref(true)
const filter = ref('')
const profile = ref({ name: '作者' })

const categories = computed(() => {
  const set = new Set(posts.value.map(p => p.category).filter(Boolean))
  return [...set]
})

const filteredPosts = computed(() => {
  if (!filter.value) return posts.value
  return posts.value.filter(p => p.category === filter.value)
})

onMounted(async () => {
  try {
    const [postsRes, profileRes] = await Promise.all([
      fetch('/api/posts'),
      fetch('/api/profile'),
    ])
    posts.value = await postsRes.json()
    if (profileRes.ok) profile.value = await profileRes.json()
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
  border: 1px solid #e5e7eb;
  color: #6b7280;
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
  color: white;
}
.category-badge {
  font-size: 0.75rem;
  padding: 0.125rem 0.5rem;
  border-radius: 9999px;
  background: #f3f4f6;
  color: #6b7280;
}
.tag-pill {
  font-size: 0.75rem;
  padding: 0.125rem 0.5rem;
  border-radius: 9999px;
  background: #eff6ff;
  color: #2563eb;
}
</style>
