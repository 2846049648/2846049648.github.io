# 项目结构文档

```
2846049648.github.io/
├── .github/
│   └── workflows/
│       └── deploy.yml              # GitHub Actions: push 到 main 自动构建部署到 Pages
│
├── public/
│   ├── images/                     # 上传的图片（通过管理后台媒体库上传）
│   └── files/                      # 上传的下载文件（通过管理后台文件库上传）
│
├── src/
│   ├── assets/
│   │   └── style.css               # 全局样式（Tailwind CSS 入口）
│   │
│   ├── posts/                      # 文章存储目录（.md 文件）
│   │   └── hello-world.md          # 示例文章
│   │
│   ├── router/
│   │   └── index.js                # Vue Router 配置（前台 + 后台路由）
│   │
│   ├── views/                      # 页面组件
│   │   ├── FrontendLayout.vue      # 前台布局（导航栏 + 页脚）
│   │   ├── Home.vue                # 首页
│   │   ├── Posts.vue               # 文章列表（分类筛选 + 作者头像）
│   │   ├── PostDetail.vue          # 文章详情（Markdown 渲染 + Utterances 评论）
│   │   ├── Downloads.vue           # 文件下载页
│   │   ├── About.vue               # 关于页面（个人资料展示）
│   │   ├── Gallery.vue             # 相册展示页
│   │   │
│   │   └── admin/                  # 管理后台页面
│   │       ├── AdminLayout.vue     # 后台布局（侧边栏导航）
│   │       ├── Dashboard.vue       # 概览 + 一键发布
│   │       ├── PostsManager.vue    # 文章管理列表
│   │       ├── PostEditor.vue      # Markdown 编辑器（编辑/预览双栏）
│   │       ├── MediaManager.vue    # 媒体库管理（图片上传）
│   │       ├── FilesManager.vue    # 文件库管理（文件上传）
│   │       ├── ProfileSettings.vue # 个人资料设置
│   │       └── GalleryManager.vue  # 相册管理
│   │
│   ├── profile.json                # 个人资料数据（姓名、头像、简介等）
│   ├── albums.json                 # 相册数据
│   ├── App.vue                     # 根组件
│   └── main.js                     # 应用入口
│
├── vite-plugin-local-api.js        # Vite 插件：提供本地 API（文章CRUD、上传、发布）
├── vite.config.js                  # Vite 配置
├── package.json                    # 依赖管理
├── index.html                      # HTML 入口
├── .gitignore
├── PROJECT_STRUCTURE.md            # 本文件
└── README.md
```

## 路由结构

```
/                   → 首页
/posts              → 文章列表
/posts/:slug        → 文章详情
/downloads          → 文件下载
/about              → 关于
/gallery            → 个人相册

/admin              → 管理面板（仅开发环境可访问）
/admin/posts        → 文章管理
/admin/posts/new    → 写新文章
/admin/posts/:slug/edit → 编辑文章
/admin/media        → 媒体库
/admin/files        → 文件库
/admin/profile      → 个人资料
/admin/gallery      → 相册管理
```

## 本地 API 接口

由 `vite-plugin-local-api.js` 在 `npm run dev` 时提供：

| 方法 | 路径 | 功能 |
|------|------|------|
| GET | /api/posts | 获取文章列表 |
| GET | /api/posts/:slug | 获取单篇文章 |
| POST | /api/posts | 创建/更新文章 |
| DELETE | /api/posts/:slug | 删除文章 |
| GET | /api/media | 获取媒体文件列表 |
| POST | /api/upload/image | 上传图片 |
| GET | /api/files | 获取下载文件列表 |
| POST | /api/upload/file | 上传文件 |
| GET | /api/profile | 获取个人资料 |
| POST | /api/profile | 更新个人资料 |
| GET | /api/albums | 获取相册列表 |
| POST | /api/albums | 创建/更新相册 |
| DELETE | /api/albums/:name | 删除相册 |
| POST | /api/upload/photo | 上传相册照片 |
| GET | /api/stats | 获取统计数据 |
| POST | /api/publish | 一键发布 |

## 数据存储

- **文章**: `src/posts/*.md` — Markdown 文件，Front Matter 存元数据
- **图片**: `public/images/*` — 通过媒体库上传
- **文件**: `public/files/*` — 通过文件库上传
- **相册**: `src/albums.json` — 相册元数据
- **照片**: `public/images/gallery/*` — 相册照片
- **个人资料**: `src/profile.json` — 昵称、头像、简介等

## 环境变量

| 变量 | 说明 | 必需 |
|------|------|------|
| VITE_UTTERANCES_REPO | GitHub 仓库名（如 user/repo），启用 Utterances 评论 | 可选 |

## Utterances 评论配置

1. 确保 GitHub 仓库是 **public**
2. 安装 [Utterances GitHub App](https://github.com/apps/utterances)
3. 在本地设置环境变量启动：

```bash
VITE_UTTERANCES_REPO="你的用户名/仓库名" npm run dev
```

评论会以 GitHub Issue 形式出现在你的仓库中。

## 开发工作流

```bash
npm run dev      # 启动本地开发 → http://localhost:5173
                 # 前台: /         后台: /admin

# 在管理后台写文章 → 上传图片 → 点击「一键发布」
# 或手动:
npm run build    # 构建
git push         # 触发 GitHub Actions 自动部署
```
