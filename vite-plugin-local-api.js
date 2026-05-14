import fs from 'fs'
import path from 'path'
import { fileURLToPath } from 'url'
import Busboy from 'busboy'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const ROOT = path.resolve(__dirname, '.')
const POSTS_DIR = path.resolve(ROOT, 'src/posts')
const PUBLIC_DIR = path.resolve(ROOT, 'public')
const IMAGES_DIR = path.resolve(PUBLIC_DIR, 'images')
const FILES_DIR = path.resolve(PUBLIC_DIR, 'files')
const PROFILE_PATH = path.resolve(ROOT, 'src/profile.json')
const ALBUMS_PATH = path.resolve(ROOT, 'src/albums.json')
const GALLERY_DIR = path.resolve(PUBLIC_DIR, 'images/gallery')

function ensureDir(dir) {
  if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true })
}

function parseFrontmatter(text) {
  const match = text.match(/^---\n([\s\S]*?)\n---\n([\s\S]*)$/)
  if (!match) return { data: {}, content: text }
  const data = {}
  match[1].split('\n').forEach(line => {
    const idx = line.indexOf(':')
    if (idx === -1) return
    const key = line.slice(0, idx).trim()
    const val = line.slice(idx + 1).trim()
    if (val.startsWith('[') && val.endsWith(']')) {
      data[key] = val.slice(1, -1).split(',').map(s => s.trim().replace(/['"]/g, ''))
    } else {
      data[key] = val.replace(/^['"]|['"]$/g, '')
    }
  })
  return { data, content: match[2] }
}

function buildFrontmatter(data) {
  let fm = '---\n'
  for (const [key, val] of Object.entries(data)) {
    if (Array.isArray(val)) {
      fm += `${key}: [${val.map(v => `"${v}"`).join(', ')}]\n`
    } else {
      fm += `${key}: ${val}\n`
    }
  }
  fm += '---\n'
  return fm
}

function getAllPosts() {
  ensureDir(POSTS_DIR)
  const files = fs.readdirSync(POSTS_DIR).filter(f => f.endsWith('.md'))
  return files.map(file => {
    const raw = fs.readFileSync(path.join(POSTS_DIR, file), 'utf-8')
    const { data } = parseFrontmatter(raw)
    return {
      slug: file.replace('.md', ''),
      ...data,
      excerpt: data.excerpt || '',
    }
  }).sort((a, b) => (a.date < b.date ? 1 : -1))
}

function getMediaFiles() {
  ensureDir(IMAGES_DIR)
  return fs.readdirSync(IMAGES_DIR)
    .filter(file => fs.statSync(path.join(IMAGES_DIR, file)).isFile())
    .map(file => ({
      name: file,
      url: `/images/${file}`,
    }))
}

function getDownloadFiles() {
  ensureDir(FILES_DIR)
  return fs.readdirSync(FILES_DIR).map(file => {
    const stat = fs.statSync(path.join(FILES_DIR, file))
    const size = stat.size > 1024 * 1024
      ? (stat.size / 1024 / 1024).toFixed(1) + ' MB'
      : (stat.size / 1024).toFixed(1) + ' KB'
    return { name: file, size, url: `/files/${file}` }
  })
}

function readJsonBody(req) {
  return new Promise((resolve) => {
    let body = ''
    req.on('data', chunk => (body += chunk))
    req.on('end', () => {
      try { resolve(JSON.parse(body)) }
      catch { resolve({}) }
    })
  })
}

function handleUpload(req, uploadDir) {
  return new Promise((resolve, reject) => {
    const busboy = Busboy({ headers: req.headers })
    let resolved = false

    busboy.on('file', (fieldname, file, { filename }) => {
      const safeName = Date.now() + '-' + filename.replace(/[^a-zA-Z0-9._-]/g, '')
      const chunks = []
      file.on('data', (data) => chunks.push(data))
      file.on('end', () => {
        const buf = Buffer.concat(chunks)
        try {
          fs.writeFileSync(path.join(uploadDir, safeName), buf)
          if (!resolved) { resolved = true; resolve(safeName) }
        } catch (err) {
          if (!resolved) { resolved = true; reject(err) }
        }
      })
    })

    busboy.on('finish', () => {
      if (!resolved) { resolved = true; reject(new Error('no file')) }
    })

    busboy.on('error', (err) => {
      if (!resolved) { resolved = true; reject(err) }
    })

    req.pipe(busboy)
  })
}

function handlePhotoUpload(req) {
  return new Promise((resolve, reject) => {
    const busboy = Busboy({ headers: req.headers })
    let albumName = ''
    let resolved = false

    busboy.on('field', (name, val) => {
      if (name === 'album') albumName = val
    })

    busboy.on('file', (fieldname, file, { filename }) => {
      const safeName = Date.now() + '-' + filename.replace(/[^a-zA-Z0-9._-]/g, '')
      const albumDir = path.join(GALLERY_DIR, albumName || 'default')
      ensureDir(albumDir)
      const chunks = []
      file.on('data', (data) => chunks.push(data))
      file.on('end', () => {
        const buf = Buffer.concat(chunks)
        try {
          fs.writeFileSync(path.join(albumDir, safeName), buf)
          if (!resolved) { resolved = true; resolve({ albumName: albumName || 'default', fileName: safeName }) }
        } catch (err) {
          if (!resolved) { resolved = true; reject(err) }
        }
      })
    })

    busboy.on('finish', () => {
      if (!resolved) { resolved = true; reject(new Error('no file')) }
    })
    busboy.on('error', (err) => {
      if (!resolved) { resolved = true; reject(err) }
    })
    req.pipe(busboy)
  })
}

export default function localApiPlugin() {
  return {
    name: 'local-api',
    configureServer(server) {
      server.middlewares.use(async (req, res, next) => {
        if (!req.url.startsWith('/api/')) return next()

        res.setHeader('Content-Type', 'application/json')
        const url = new URL(req.url, `http://${req.headers.host}`)
        const method = req.method

        try {
          // === Posts API ===
          if (url.pathname === '/api/posts' && method === 'GET') {
            res.end(JSON.stringify(getAllPosts()))
            return
          }

          if (url.pathname === '/api/posts' && method === 'POST') {
            const body = await readJsonBody(req)
            const { slug, title, date, category, tags, excerpt, content } = body
            if (!slug || !title) {
              res.statusCode = 400
              res.end(JSON.stringify({ error: 'slug and title are required' }))
              return
            }
            const fm = buildFrontmatter({
              title, date: date || new Date().toISOString().split('T')[0],
              category: category || '',
              tags: tags || [],
              excerpt: excerpt || '',
            })
            fs.writeFileSync(path.join(POSTS_DIR, `${slug}.md`), fm + (content || ''), 'utf-8')
            res.end(JSON.stringify({ ok: true, slug }))
            return
          }

          const postMatch = url.pathname.match(/^\/api\/posts\/(.+)$/)
          if (postMatch) {
            const slug = postMatch[1]
            const filePath = path.join(POSTS_DIR, `${slug}.md`)

            if (method === 'GET') {
              if (!fs.existsSync(filePath)) {
                res.statusCode = 404
                res.end(JSON.stringify({ error: 'not found' }))
                return
              }
              const raw = fs.readFileSync(filePath, 'utf-8')
              const { data, content } = parseFrontmatter(raw)
              res.end(JSON.stringify({ slug, ...data, content }))
              return
            }

            if (method === 'DELETE') {
              if (fs.existsSync(filePath)) fs.unlinkSync(filePath)
              res.end(JSON.stringify({ ok: true }))
              return
            }
          }

          // === Media API ===
          if (url.pathname === '/api/media' && method === 'GET') {
            res.end(JSON.stringify(getMediaFiles()))
            return
          }

          const mediaMatch = url.pathname.match(/^\/api\/media\/(.+)$/)
          if (mediaMatch && method === 'DELETE') {
            const fileName = decodeURIComponent(mediaMatch[1])
            const filePath = path.join(IMAGES_DIR, fileName)
            if (fs.existsSync(filePath)) {
              const stat = fs.statSync(filePath)
              if (stat.isDirectory()) {
                fs.rmSync(filePath, { recursive: true, force: true })
              } else {
                fs.unlinkSync(filePath)
              }
            }
            res.end(JSON.stringify({ ok: true }))
            return
          }

          if (url.pathname === '/api/upload/image' && method === 'POST') {
            ensureDir(IMAGES_DIR)
            const fileName = await handleUpload(req, IMAGES_DIR)
            res.end(JSON.stringify({ url: `/images/${fileName}` }))
            return
          }

          // === Files API ===
          if (url.pathname === '/api/files' && method === 'GET') {
            res.end(JSON.stringify(getDownloadFiles()))
            return
          }

          if (url.pathname === '/api/upload/file' && method === 'POST') {
            ensureDir(FILES_DIR)
            const fileName = await handleUpload(req, FILES_DIR)
            res.end(JSON.stringify({ url: `/files/${fileName}` }))
            return
          }

          const filesMatch = url.pathname.match(/^\/api\/files\/(.+)$/)
          if (filesMatch && method === 'DELETE') {
            const fileName = decodeURIComponent(filesMatch[1])
            const filePath = path.join(FILES_DIR, fileName)
            if (fs.existsSync(filePath)) {
              fs.unlinkSync(filePath)
            }
            res.end(JSON.stringify({ ok: true }))
            return
          }

          // === Profile API ===
          if (url.pathname === '/api/profile') {
            if (method === 'GET') {
              if (fs.existsSync(PROFILE_PATH)) {
                res.end(fs.readFileSync(PROFILE_PATH, 'utf-8'))
              } else {
                res.end(JSON.stringify({
                  name: '作者', avatar: '',
                  bio: '这个人很懒，什么都没写…',
                  email: '', social: {},
                }))
              }
              return
            }
            if (method === 'POST') {
              const body = await readJsonBody(req)
              fs.writeFileSync(PROFILE_PATH, JSON.stringify(body, null, 2), 'utf-8')
              res.end(JSON.stringify({ ok: true }))
              return
            }
          }

          // === Albums API ===
          if (url.pathname === '/api/albums') {
            if (method === 'GET') {
              if (fs.existsSync(ALBUMS_PATH)) {
                res.end(fs.readFileSync(ALBUMS_PATH, 'utf-8'))
              } else {
                res.end('[]')
              }
              return
            }
            if (method === 'POST') {
              const body = await readJsonBody(req)
              let albums = []
              if (fs.existsSync(ALBUMS_PATH)) {
                albums = JSON.parse(fs.readFileSync(ALBUMS_PATH, 'utf-8'))
              }
              const idx = albums.findIndex(a => a.name === body.name)
              if (idx >= 0) {
                albums[idx] = body
              } else {
                albums.push(body)
              }
              fs.writeFileSync(ALBUMS_PATH, JSON.stringify(albums, null, 2), 'utf-8')
              res.end(JSON.stringify({ ok: true }))
              return
            }
          }

          const albumNameMatch = url.pathname.match(/^\/api\/albums\/(.+)$/)
          if (albumNameMatch && method === 'DELETE') {
            const name = decodeURIComponent(albumNameMatch[1])
            let albums = []
            if (fs.existsSync(ALBUMS_PATH)) {
              albums = JSON.parse(fs.readFileSync(ALBUMS_PATH, 'utf-8'))
            }
            albums = albums.filter(a => a.name !== name)
            fs.writeFileSync(ALBUMS_PATH, JSON.stringify(albums, null, 2), 'utf-8')
            // Remove gallery folder
            const albumDir = path.join(GALLERY_DIR, name)
            if (fs.existsSync(albumDir)) {
              fs.rmSync(albumDir, { recursive: true, force: true })
            }
            res.end(JSON.stringify({ ok: true }))
            return
          }

          // === Photo upload (multipart: album + file) ===
          if (url.pathname === '/api/upload/photo' && method === 'POST') {
            try {
              const { albumName, fileName: uploadedName } = await handlePhotoUpload(req)
              const photoUrl = `/images/gallery/${albumName}/${uploadedName}`
              // Update albums.json to include the new photo
              let albums = []
              if (fs.existsSync(ALBUMS_PATH)) {
                albums = JSON.parse(fs.readFileSync(ALBUMS_PATH, 'utf-8'))
              }
              const album = albums.find(a => a.name === albumName)
              if (album) {
                if (!album.photos) album.photos = []
                album.photos.push(photoUrl)
                fs.writeFileSync(ALBUMS_PATH, JSON.stringify(albums, null, 2), 'utf-8')
              }
              res.end(JSON.stringify({ url: photoUrl }))
            } catch (e) {
              res.statusCode = 400
              res.end(JSON.stringify({ error: e.message }))
            }
            return
          }

          // === Stats API ===
          if (url.pathname === '/api/stats' && method === 'GET') {
            let albums = []
            if (fs.existsSync(ALBUMS_PATH)) {
              albums = JSON.parse(fs.readFileSync(ALBUMS_PATH, 'utf-8'))
            }
            res.end(JSON.stringify({
              posts: getAllPosts().length,
              media: getMediaFiles().length,
              files: getDownloadFiles().length,
              albums: albums.length,
            }))
            return
          }

          // === Publish API ===
          if (url.pathname === '/api/publish' && method === 'POST') {
            const { execSync } = await import('child_process')
            try {
              execSync('npm run build', { cwd: ROOT, stdio: 'inherit' })
              const msgBody = await readJsonBody(req)
              const commitMsg = msgBody.message || 'update site'
              execSync('git add -A', { cwd: ROOT, stdio: 'inherit' })
              execSync(`git commit -m "${commitMsg.replace(/"/g, '\\"')}"`, { cwd: ROOT, stdio: 'inherit' })
              execSync('git push', { cwd: ROOT, stdio: 'inherit' })
              res.end(JSON.stringify({ ok: true, message: '发布成功！' }))
            } catch (e) {
              res.statusCode = 500
              res.end(JSON.stringify({ error: e.message }))
            }
            return
          }

          res.statusCode = 404
          res.end(JSON.stringify({ error: 'api not found' }))
        } catch (e) {
          res.statusCode = 500
          res.end(JSON.stringify({ error: e.message }))
        }
      })
    },
  }
}
