<template>
  <div>
    <h2 class="text-3xl font-bold text-gray-900 mb-2">个人相册</h2>
    <p class="text-gray-500 mb-8">记录生活中的美好瞬间</p>

    <div v-if="loading" class="text-center py-16 text-gray-400">
      <svg class="animate-spin w-8 h-8 mx-auto mb-3" fill="none" viewBox="0 0 24 24"><circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"/><path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4z"/></svg>
      加载中...
    </div>
    <div v-else-if="albums.length === 0" class="text-center py-16 text-gray-400">
      <svg class="w-12 h-12 mx-auto mb-3 text-gray-300" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="1.5" d="M4 16l4.586-4.586a2 2 0 012.828 0L16 16m-2-2l1.586-1.586a2 2 0 012.828 0L20 14m-6-6h.01M6 20h12a2 2 0 002-2V6a2 2 0 00-2-2H6a2 2 0 00-2 2v12a2 2 0 002 2z"/></svg>
      暂无相册
    </div>
    <template v-else>
      <!-- Album cards -->
      <div v-if="!currentAlbum" class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-6">
        <div
          v-for="album in albums" :key="album.name"
          class="group cursor-pointer bg-white rounded-xl overflow-hidden shadow-sm hover:shadow-lg transition-all border border-gray-100"
          @click="openAlbum(album)"
        >
          <div class="aspect-[4/3] bg-gray-100 overflow-hidden relative">
            <img v-if="album.cover" :src="album.cover" class="w-full h-full object-cover group-hover:scale-105 transition-transform duration-500" />
            <div v-else class="w-full h-full flex items-center justify-center text-gray-300">
              <svg class="w-12 h-12" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="1.5" d="M4 16l4.586-4.586a2 2 0 012.828 0L16 16m-2-2l1.586-1.586a2 2 0 012.828 0L20 14m-6-6h.01M6 20h12a2 2 0 002-2V6a2 2 0 00-2-2H6a2 2 0 00-2 2v12a2 2 0 002 2z"/></svg>
            </div>
            <div class="absolute inset-0 bg-gradient-to-t from-black/40 to-transparent opacity-0 group-hover:opacity-100 transition-opacity" />
            <div class="absolute bottom-3 right-3 bg-black/50 text-white text-xs px-2 py-1 rounded-full">
              {{ album.photos ? album.photos.length : 0 }} 张
            </div>
          </div>
          <div class="p-4">
            <h3 class="font-semibold text-gray-900 group-hover:text-blue-600 transition-colors">{{ album.title }}</h3>
            <p v-if="album.description" class="text-sm text-gray-500 mt-1 line-clamp-1">{{ album.description }}</p>
          </div>
        </div>
      </div>

      <!-- Album detail (photos grid) -->
      <div v-else>
        <button @click="currentAlbum = null" class="inline-flex items-center text-gray-500 hover:text-blue-600 transition-colors mb-6 group">
          <svg class="w-4 h-4 mr-1 group-hover:-translate-x-0.5 transition-transform" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M15 19l-7-7 7-7"/></svg>
          返回相册列表
        </button>
        <div class="mb-6">
          <h3 class="text-2xl font-bold text-gray-900">{{ currentAlbum.title }}</h3>
          <p v-if="currentAlbum.description" class="text-gray-500 mt-1">{{ currentAlbum.description }}</p>
        </div>
        <div class="grid grid-cols-2 sm:grid-cols-3 lg:grid-cols-4 gap-3">
          <div
            v-for="(photo, idx) in currentAlbum.photos" :key="idx"
            class="aspect-square bg-gray-100 rounded-lg overflow-hidden cursor-pointer group relative"
            @click="openLightbox(idx)"
          >
            <img :src="photo" class="w-full h-full object-cover group-hover:scale-105 transition-transform duration-500" loading="lazy" />
            <div class="absolute inset-0 bg-black/0 group-hover:bg-black/10 transition-colors" />
          </div>
        </div>
      </div>
    </template>

    <!-- Lightbox -->
    <teleport to="body">
      <div
        v-if="lightboxVisible"
        class="fixed inset-0 z-[999] bg-black/90 flex items-center justify-center"
        @click="closeLightbox"
      >
        <button class="absolute top-4 right-4 text-white/70 hover:text-white text-3xl leading-none z-10">&times;</button>
        <button
          v-if="lightboxIdx > 0"
          class="absolute left-4 top-1/2 -translate-y-1/2 text-white/70 hover:text-white text-4xl leading-none z-10"
          @click.stop="lightboxIdx--"
        >&lsaquo;</button>
        <button
          v-if="lightboxIdx < photosLength - 1"
          class="absolute right-4 top-1/2 -translate-y-1/2 text-white/70 hover:text-white text-4xl leading-none z-10"
          @click.stop="lightboxIdx++"
        >&rsaquo;</button>
        <img
          :src="currentPhoto"
          class="max-w-[90vw] max-h-[90vh] object-contain select-none"
          @click.stop
        />
        <div class="absolute bottom-6 text-white/60 text-sm">
          {{ lightboxIdx + 1 }} / {{ photosLength }}
        </div>
      </div>
    </teleport>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'

const albums = ref([])
const loading = ref(true)
const currentAlbum = ref(null)
const lightboxVisible = ref(false)
const lightboxIdx = ref(0)

const photosLength = computed(() => currentAlbum.value?.photos?.length || 0)
const currentPhoto = computed(() => {
  if (!currentAlbum.value?.photos) return ''
  return currentAlbum.value.photos[lightboxIdx.value]
})

onMounted(async () => {
  try {
    const res = await fetch('/api/albums')
    albums.value = await res.json()
  } catch {
    albums.value = []
  } finally {
    loading.value = false
  }
})

function openAlbum(album) {
  currentAlbum.value = album
  lightboxIdx.value = 0
}

function openLightbox(idx) {
  lightboxIdx.value = idx
  lightboxVisible.value = true
}

function closeLightbox() {
  lightboxVisible.value = false
}

// Keyboard navigation
document.addEventListener('keydown', (e) => {
  if (!lightboxVisible.value) return
  if (e.key === 'Escape') closeLightbox()
  if (e.key === 'ArrowLeft' && lightboxIdx.value > 0) lightboxIdx.value--
  if (e.key === 'ArrowRight' && lightboxIdx.value < photosLength.value - 1) lightboxIdx.value++
})
</script>
