<template>
  <div class="min-h-screen flex" style="background: #f0f2f5;">
    <!-- Dark cyber sidebar -->
    <el-menu
      :default-active="activeMenu"
      class="w-60 min-h-screen border-r"
      style="background: #0f1929; border-color: #1e3048;"
      router
      text-color="#6b8cae"
      active-text-color="#00e5ff"
    >
      <div class="p-4 text-lg font-bold border-b flex items-center gap-2" style="border-color: #1e3048;">
        <span class="gradient-text">管理面板</span>
      </div>
      <el-menu-item index="/admin" style="--el-menu-hover-bg-color: rgba(0,229,255,0.05);">
        <span>概览</span>
      </el-menu-item>
      <el-menu-item index="/admin/posts" style="--el-menu-hover-bg-color: rgba(0,229,255,0.05);">
        <span>文章管理</span>
      </el-menu-item>
      <el-menu-item index="/admin/media" style="--el-menu-hover-bg-color: rgba(0,229,255,0.05);">
        <span>媒体库</span>
      </el-menu-item>
      <el-menu-item index="/admin/files" style="--el-menu-hover-bg-color: rgba(0,229,255,0.05);">
        <span>文件库</span>
      </el-menu-item>
      <el-menu-item index="/admin/gallery" style="--el-menu-hover-bg-color: rgba(0,229,255,0.05);">
        <span>相册管理</span>
      </el-menu-item>
      <el-menu-item index="/admin/profile" style="--el-menu-hover-bg-color: rgba(0,229,255,0.05);">
        <span>个人资料</span>
      </el-menu-item>
      <el-divider class="my-2" style="border-color: #1e3048;" />
      <el-menu-item index="/" style="--el-menu-hover-bg-color: rgba(0,229,255,0.05);">
        <span>返回前台</span>
      </el-menu-item>
    </el-menu>
    <!-- Light main content area -->
    <main class="flex-1 p-8 overflow-auto" style="background: #f0f2f5;">
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

<style>
/* Sidebar menu overrides */
.el-menu-item.is-active {
  background: rgba(0, 229, 255, 0.08) !important;
  border-right: 2px solid var(--color-primary);
}
.el-menu-item:hover {
  background: rgba(0, 229, 255, 0.04) !important;
}
.el-divider {
  border-color: #1e3048 !important;
}
</style>
