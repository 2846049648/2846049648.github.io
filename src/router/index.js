import { createRouter, createWebHistory } from 'vue-router'

const routes = [
  {
    path: '/',
    component: () => import('../views/FrontendLayout.vue'),
    children: [
      {
        path: '',
        name: 'Home',
        component: () => import('../views/Home.vue'),
      },
      {
        path: 'posts',
        name: 'Posts',
        component: () => import('../views/Posts.vue'),
      },
      {
        path: 'posts/:slug',
        name: 'PostDetail',
        component: () => import('../views/PostDetail.vue'),
      },
      {
        path: 'downloads',
        name: 'Downloads',
        component: () => import('../views/Downloads.vue'),
      },
      {
        path: 'about',
        name: 'About',
        component: () => import('../views/About.vue'),
      },
      {
        path: 'gallery',
        name: 'Gallery',
        component: () => import('../views/Gallery.vue'),
      },
    ],
  },
  {
    path: '/admin',
    component: () => import('../views/admin/AdminLayout.vue'),
    beforeEnter: () => {
      if (import.meta.env.PROD) {
        return '/'
      }
    },
    children: [
      {
        path: '',
        name: 'Dashboard',
        component: () => import('../views/admin/Dashboard.vue'),
      },
      {
        path: 'posts',
        name: 'AdminPosts',
        component: () => import('../views/admin/PostsManager.vue'),
      },
      {
        path: 'posts/new',
        name: 'AdminPostNew',
        component: () => import('../views/admin/PostEditor.vue'),
      },
      {
        path: 'posts/:slug/edit',
        name: 'AdminPostEdit',
        component: () => import('../views/admin/PostEditor.vue'),
      },
      {
        path: 'media',
        name: 'AdminMedia',
        component: () => import('../views/admin/MediaManager.vue'),
      },
      {
        path: 'files',
        name: 'AdminFiles',
        component: () => import('../views/admin/FilesManager.vue'),
      },
      {
        path: 'gallery',
        name: 'AdminGallery',
        component: () => import('../views/admin/GalleryManager.vue'),
      },
      {
        path: 'profile',
        name: 'AdminProfile',
        component: () => import('../views/admin/ProfileSettings.vue'),
      },
    ],
  },
  {
    path: '/:pathMatch(.*)*',
    redirect: '/',
  },
]

const router = createRouter({
  history: createWebHistory(),
  routes,
})

export default router
