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
})
