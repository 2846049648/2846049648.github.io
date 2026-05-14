import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import tailwindcss from '@tailwindcss/vite'
import localApi from './vite-plugin-local-api.js'

export default defineConfig({
  plugins: [vue(), tailwindcss(), localApi()],
  base: '/',
  server: {
    host: true,
  },
  build: {
    rollupOptions: {
      output: {
        manualChunks(id) {
          if (id.includes('highlight.js')) return 'vendor-highlight'
          if (id.includes('element-plus')) return 'vendor-element'
          if (id.includes('marked')) return 'vendor-marked'
          if (id.includes('exifr')) return 'vendor-exifr'
        },
      },
    },
  },
})
