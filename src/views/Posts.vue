<template>
  <div>
    <h2 class="text-3xl font-bold mb-6">最新文章</h2>

    <!-- Category / Tag filter -->
    <div class="flex flex-wrap gap-2 mb-6">
      <el-tag :type="!filter ? 'primary' : 'info'" class="cursor-pointer" @click="filter = ''">全部</el-tag>
      <el-tag v-for="cat in categories" :key="cat" :type="filter === cat ? 'primary' : 'info'"
        class="cursor-pointer" @click="filter = cat">{{ cat }}</el-tag>
    </div>

    <div v-if="loading" class="text-center py-12 text-gray-500">加载中...</div>
    <div v-else-if="filteredPosts.length === 0" class="text-center py-12 text-gray-500">暂无文章</div>
    <div v-else class="space-y-6">
      <article
        v-for="post in filteredPosts"
        :key="post.slug"
        class="bg-white rounded-lg shadow p-6 hover:shadow-md transition-shadow"
      >
        <router-link :to="`/posts/${post.slug}`" class="block">
          <div class="flex items-start space-x-4">
            <div class="w-10 h-10 rounded-full bg-blue-100 flex-shrink-0 flex items-center justify-center text-blue-600 font-bold">
              {{ (profile.name || '作').charAt(0) }}
            </div>
            <div class="flex-1 min-w-0">
              <h3 class="text-xl font-semibold text-gray-900 mb-1">{{ post.title }}</h3>
              <p class="text-gray-600 mb-3 line-clamp-2">{{ post.excerpt }}</p>
              <div class="flex items-center text-sm text-gray-500 space-x-4">
                <span>{{ post.date }}</span>
                <span v-if="post.category" class="bg-gray-100 text-gray-600 px-2 py-0.5 rounded">{{ post.category }}</span>
                <span v-for="tag in post.tags" :key="tag" class="bg-blue-100 text-blue-600 px-2 py-0.5 rounded">{{ tag }}</span>
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
