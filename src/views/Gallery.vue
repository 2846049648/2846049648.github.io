<template>
  <div>
    <h2 class="text-3xl font-bold mb-2" :style="{ color: 'var(--text-primary)' }">个人相册</h2>
    <p class="mb-8" :style="{ color: 'var(--text-muted)' }">记录生活中的美好瞬间</p>

    <div v-if="loading" class="text-center py-16" :style="{ color: 'var(--text-light)' }">
      <svg class="animate-spin w-8 h-8 mx-auto mb-3" fill="none" viewBox="0 0 24 24"><circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"/><path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4z"/></svg>
      加载中...
    </div>
    <div v-else-if="albums.length === 0" class="text-center py-16" :style="{ color: 'var(--text-light)' }">
      <svg class="w-12 h-12 mx-auto mb-3" :style="{ color: 'var(--text-light)' }" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="1.5" d="M4 16l4.586-4.586a2 2 0 012.828 0L16 16m-2-2l1.586-1.586a2 2 0 012.828 0L20 14m-6-6h.01M6 20h12a2 2 0 002-2V6a2 2 0 00-2-2H6a2 2 0 00-2 2v12a2 2 0 002 2z"/></svg>
      暂无相册
    </div>
    <template v-else>
      <!-- Album list view -->
      <div v-if="!currentAlbum" class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-6">
        <div
          v-for="album in albums" :key="album.name"
          class="group cursor-pointer rounded-xl overflow-hidden shadow-sm hover:shadow-lg transition-all border"
          :style="{ background: 'var(--bg-surface)', borderColor: 'var(--border-color)' }"
          @click="openAlbum(album)"
        >
          <div class="aspect-[4/3] overflow-hidden relative" :style="{ background: 'var(--bg-muted)' }">
            <img v-if="album.cover" :src="album.cover" class="w-full h-full object-cover group-hover:scale-105 transition-transform duration-500" />
            <div v-else class="w-full h-full flex items-center justify-center" :style="{ color: 'var(--text-light)' }">
              <svg class="w-12 h-12" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="1.5" d="M4 16l4.586-4.586a2 2 0 012.828 0L16 16m-2-2l1.586-1.586a2 2 0 012.828 0L20 14m-6-6h.01M6 20h12a2 2 0 002-2V6a2 2 0 00-2-2H6a2 2 0 00-2 2v12a2 2 0 002 2z"/></svg>
            </div>
            <div class="absolute inset-0 bg-gradient-to-t from-black/40 to-transparent opacity-0 group-hover:opacity-100 transition-opacity" />
            <div class="absolute bottom-3 right-3 bg-black/50 text-white text-xs px-2 py-1 rounded-full">
              {{ album.photos ? album.photos.length : 0 }} 张
            </div>
          </div>
          <div class="p-4">
            <h3 class="font-semibold group-hover:text-blue-600 transition-colors" :style="{ color: 'var(--text-primary)' }">{{ album.title }}</h3>
            <p v-if="album.description" class="text-sm mt-1 line-clamp-1" :style="{ color: 'var(--text-muted)' }">{{ album.description }}</p>
          </div>
        </div>
      </div>

      <!-- Album detail -->
      <div v-else>
        <button @click="closeAlbum" class="inline-flex items-center transition-colors mb-4 group" :style="{ color: 'var(--text-muted)' }">
          <svg class="w-4 h-4 mr-1 group-hover:-translate-x-0.5 transition-transform" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M15 19l-7-7 7-7"/></svg>
          返回相册列表
        </button>
        <div class="mb-6">
          <h3 class="text-2xl font-bold" :style="{ color: 'var(--text-primary)' }">{{ currentAlbum.title }}</h3>
          <p v-if="currentAlbum.description" class="mt-1" :style="{ color: 'var(--text-muted)' }">{{ currentAlbum.description }}</p>
          <!-- Photo essay (album.story as markdown) -->
          <div v-if="currentAlbum.story" class="mt-4 prose prose-sm max-w-none" :style="{ color: 'var(--text-muted)' }" v-html="renderedStory" />
        </div>

        <!-- Masonry grid -->
        <div class="masonry-grid">
          <div
            v-for="(photo, idx) in currentAlbum.photos" :key="idx"
            class="masonry-item group relative overflow-hidden rounded-lg cursor-pointer"
            :style="{ background: 'var(--bg-muted)' }"
            @click="openLightbox(idx)"
          >
            <img
              :src="getPhotoUrl(photo)"
              :alt="getPhotoAlt(photo)"
              class="w-full h-auto object-cover group-hover:scale-105 transition-transform duration-500"
              loading="lazy"
            />
            <div class="absolute inset-0 bg-black/0 group-hover:bg-black/10 transition-colors" />
          </div>
        </div>
      </div>
    </template>

    <!-- PhotoSwipe lightbox overlay -->
    <div
      v-if="psVisible"
      class="fixed inset-0 z-[999] bg-black flex flex-col"
      @keydown="onKeydown"
      tabindex="0"
      ref="psContainerRef"
    >
      <!-- Top bar -->
      <div class="absolute top-0 left-0 right-0 z-10 flex items-center justify-between p-4 bg-gradient-to-b from-black/60 to-transparent">
        <div class="text-white text-sm">
          {{ psIndex + 1 }} / {{ currentAlbum?.photos?.length || 0 }}
        </div>
        <button @click="closePs" class="text-white/80 hover:text-white text-2xl leading-none p-1">&times;</button>
      </div>

      <!-- Main image area -->
      <div class="flex-1 flex items-center justify-center relative overflow-hidden" @wheel="onWheel" @mousemove="onPanMove" @mousedown="onPanStart" @mouseup="onPanEnd" @touchstart="onTouchStart" @touchmove="onTouchMove" @touchend="onTouchEnd">
        <img
          :src="psSrc"
          class="max-w-[95vw] max-h-[85vh] object-contain select-none transition-transform duration-200"
          :style="{ transform: `scale(${psZoom})`, cursor: psZoom > 1 ? 'grab' : 'zoom-in' }"
          @click="onImgClick"
          @load="loadExif"
          ref="psImgRef"
        />
        <!-- EXIF info overlay -->
        <div
          v-if="exifData"
          class="absolute bottom-4 left-1/2 -translate-x-1/2 bg-black/70 backdrop-blur-sm text-white text-xs px-4 py-2 rounded-lg flex flex-wrap gap-x-4 gap-y-1 justify-center max-w-[90vw]"
        >
          <span v-if="exifData.Make || exifData.Model">
            {{ [exifData.Make, exifData.Model].filter(Boolean).join(' ') }}
          </span>
          <span v-if="exifData.FNumber">ƒ/{{ exifData.FNumber }}</span>
          <span v-if="exifData.ExposureTime">1/{{ Math.round(1/exifData.ExposureTime) }}s</span>
          <span v-if="exifData.ISOSpeedRatings">ISO {{ exifData.ISOSpeedRatings }}</span>
          <span v-if="exifData.FocalLength">{{ exifData.FocalLength }}mm</span>
          <span v-if="exifData.LensModel">{{ exifData.LensModel }}</span>
        </div>
        <div v-else-if="exifLoading" class="absolute bottom-4 left-1/2 -translate-x-1/2 bg-black/50 text-white/60 text-xs px-3 py-1.5 rounded-lg">
          读取 EXIF...
        </div>
      </div>

      <!-- Nav arrows -->
      <button
        v-if="psIndex > 0"
        class="absolute left-4 top-1/2 -translate-y-1/2 text-white/70 hover:text-white text-4xl leading-none z-10 drop-shadow-lg"
        @click="psIndex--"
      >‹</button>
      <button
        v-if="psIndex < (currentAlbum?.photos?.length || 1) - 1"
        class="absolute right-4 top-1/2 -translate-y-1/2 text-white/70 hover:text-white text-4xl leading-none z-10 drop-shadow-lg"
        @click="psIndex++"
      >›</button>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, watch, onMounted, onUnmounted, nextTick } from 'vue'
import { marked } from 'marked'
import * as exifr from 'exifr'

const albums = ref([])
const loading = ref(true)
const currentAlbum = ref(null)
const renderedStory = ref('')

// Lightbox state
const psVisible = ref(false)
const psIndex = ref(0)
const psSrc = ref('')
const psZoom = ref(1)
const psImgRef = ref(null)
const psContainerRef = ref(null)
const exifData = ref(null)
const exifLoading = ref(false)

// Pan state
let isPanning = false
let panStartX = 0
let panStartY = 0
let panOffsetX = 0
let panOffsetY = 0
let imgLeft = 0
let imgTop = 0

// Touch state
let touchStartDistance = 0
let touchStartZoom = 1

onMounted(async () => {
  try {
    const url = import.meta.env.DEV ? '/api/albums' : '/albums.json'
    const res = await fetch(url)
    albums.value = await res.json()
  } catch {
    albums.value = []
  } finally {
    loading.value = false
  }
})

function getPhotoUrl(photo) {
  return typeof photo === 'string' ? photo : photo.url
}
function getPhotoAlt(photo) {
  return typeof photo === 'string' ? '' : (photo.caption || '')
}

function openAlbum(album) {
  currentAlbum.value = album
  psIndex.value = 0
  if (album.story) {
    renderedStory.value = marked.parse(album.story)
  }
}

function closeAlbum() {
  currentAlbum.value = null
  closePs()
}

function openLightbox(idx) {
  psIndex.value = idx
  psZoom.value = 1
  updatePsSrc()
  psVisible.value = true
  nextTick(() => psContainerRef.value?.focus())
}

function closePs() {
  psVisible.value = false
  exifData.value = null
}

function updatePsSrc() {
  if (!currentAlbum.value?.photos) return
  const photo = currentAlbum.value.photos[psIndex.value]
  psSrc.value = getPhotoUrl(photo)
  exifData.value = null
  exifLoading.value = true
}

async function loadExif() {
  if (!psSrc.value) return
  exifLoading.value = true
  try {
    const data = await exifr.parse(psSrc.value, {
      pick: ['Make', 'Model', 'FNumber', 'ExposureTime', 'ISOSpeedRatings', 'FocalLength', 'LensModel'],
      translateKeys: true,
    })
    if (data) {
      exifData.value = data
      // Format FNumber
      if (data.FNumber) {
        const n = parseFloat(data.FNumber)
        data.FNumber = Number.isInteger(n) ? n.toFixed(1) : String(n)
      }
      if (data.FocalLength) {
        data.FocalLength = Math.round(parseFloat(data.FocalLength))
      }
    }
  } catch {
    // EXIF not available
  } finally {
    exifLoading.value = false
  }
}

watch(psIndex, () => {
  psZoom.value = 1
  updatePsSrc()
})

// Keyboard nav
function onKeydown(e) {
  if (e.key === 'Escape') closePs()
  if (e.key === 'ArrowLeft' && psIndex.value > 0) psIndex.value--
  if (e.key === 'ArrowRight' && psIndex.value < (currentAlbum.value?.photos?.length || 1) - 1) psIndex.value++
  if (e.key === '+' || e.key === '=') psZoom.value = Math.min(4, psZoom.value + 0.25)
  if (e.key === '-') psZoom.value = Math.max(0.5, psZoom.value - 0.25)
}

// Scroll zoom
function onWheel(e) {
  e.preventDefault()
  const delta = e.deltaY > 0 ? -0.1 : 0.1
  psZoom.value = Math.max(0.5, Math.min(4, psZoom.value + delta))
}

// Click to zoom
function onImgClick() {
  if (psZoom.value > 1) {
    psZoom.value = 1
  } else {
    psZoom.value = 2
  }
}

// Mouse pan
function onPanStart(e) {
  if (psZoom.value <= 1) return
  isPanning = true
  panStartX = e.clientX
  panStartY = e.clientY
  if (psImgRef.value) {
    const rect = psImgRef.value.getBoundingClientRect()
    imgLeft = rect.left
    imgTop = rect.top
  }
}
function onPanMove(e) {
  if (!isPanning) return
  panOffsetX = e.clientX - panStartX
  panOffsetY = e.clientY - panStartY
  if (psImgRef.value) {
    psImgRef.value.style.transform = `scale(${psZoom.value}) translate(${panOffsetX / psZoom.value}px, ${panOffsetY / psZoom.value}px)`
  }
}
function onPanEnd() {
  isPanning = false
}

// Touch support
function onTouchStart(e) {
  if (e.touches.length === 2) {
    touchStartDistance = Math.hypot(
      e.touches[0].clientX - e.touches[1].clientX,
      e.touches[0].clientY - e.touches[1].clientY
    )
    touchStartZoom = psZoom.value
  }
}
function onTouchMove(e) {
  if (e.touches.length === 2) {
    e.preventDefault()
    const dist = Math.hypot(
      e.touches[0].clientX - e.touches[1].clientX,
      e.touches[0].clientY - e.touches[1].clientY
    )
    psZoom.value = Math.max(0.5, Math.min(4, touchStartZoom * (dist / touchStartDistance)))
  }
}
function onTouchEnd() {}

onUnmounted(() => {
  closePs()
})
</script>

<style scoped>
/* CSS Masonry grid */
.masonry-grid {
  columns: 2 300px;
  column-gap: 0.75rem;
}
.masonry-item {
  break-inside: avoid;
  margin-bottom: 0.75rem;
}
@media (min-width: 640px) {
  .masonry-grid {
    columns: 3 240px;
    column-gap: 1rem;
  }
  .masonry-item {
    margin-bottom: 1rem;
  }
}
@media (min-width: 1024px) {
  .masonry-grid {
    columns: 4 220px;
  }
}
</style>
