<template>
  <div class="text-center py-16">
    <h2 class="text-4xl font-bold text-gray-900 mb-4">欢迎来到我的个人空间</h2>
    <p class="text-lg text-gray-600 mb-8">分享技术文章与实用资源</p>
    <div class="flex justify-center space-x-4">
      <router-link to="/posts">
        <el-button type="primary" size="large">浏览文章</el-button>
      </router-link>
      <router-link to="/downloads">
        <el-button size="large">下载资源</el-button>
      </router-link>
    </div>
  </div>
</template>
