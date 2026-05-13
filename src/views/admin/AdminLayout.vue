<template>
  <div class="min-h-screen flex">
    <el-menu
      :default-active="activeMenu"
      class="w-60 min-h-screen border-r"
      router
    >
      <div class="p-4 text-lg font-bold text-gray-800 border-b">管理面板</div>
      <el-menu-item index="/admin">
        <span>概览</span>
      </el-menu-item>
      <el-menu-item index="/admin/posts">
        <span>文章管理</span>
      </el-menu-item>
      <el-menu-item index="/admin/media">
        <span>媒体库</span>
      </el-menu-item>
      <el-menu-item index="/admin/files">
        <span>文件库</span>
      </el-menu-item>
      <el-menu-item index="/admin/profile">
        <span>个人资料</span>
      </el-menu-item>
      <el-divider class="my-2" />
      <el-menu-item index="/">
        <span>返回前台</span>
      </el-menu-item>
    </el-menu>
    <main class="flex-1 p-8 bg-gray-50 overflow-auto">
      <router-view />
    </main>
  </div>
</template>

<script setup>
import { computed } from 'vue'
import { useRoute } from 'vue-router'

const route = useRoute()
const activeMenu = computed(() => route.path)
</script>
