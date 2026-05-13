import fs from 'fs'
import path from 'path'
import { fileURLToPath } from 'url'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const ROOT = path.resolve(__dirname, '..')

const POSTS_SRC = path.resolve(ROOT, 'src/posts')
const POSTS_DEST = path.resolve(ROOT, 'public/posts')
const FILES_DIR = path.resolve(ROOT, 'public/files')

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

console.log('Generating static data for production...\n')

// ── Posts ──
ensureDir(POSTS_DEST)
const mdFiles = fs.readdirSync(POSTS_SRC).filter(f => f.endsWith('.md'))

// Copy .md files
mdFiles.forEach(file => {
  fs.copyFileSync(path.join(POSTS_SRC, file), path.join(POSTS_DEST, file))
  console.log(`  copied: posts/${file}`)
})

// Generate index.json
const posts = mdFiles.map(file => {
  const raw = fs.readFileSync(path.join(POSTS_SRC, file), 'utf-8')
  const { data } = parseFrontmatter(raw)
  return { slug: file.replace('.md', ''), ...data, excerpt: data.excerpt || '' }
}).sort((a, b) => (a.date < b.date ? 1 : -1))

fs.writeFileSync(path.join(POSTS_DEST, 'index.json'), JSON.stringify(posts, null, 2), 'utf-8')
console.log(`  generated: posts/index.json (${posts.length} posts)`)

// ── Files (downloads) index ──
ensureDir(FILES_DIR)
const allFiles = fs.readdirSync(FILES_DIR).filter(f => f !== 'index.json')
const fileList = allFiles.map(file => {
  const stat = fs.statSync(path.join(FILES_DIR, file))
  const size = stat.size > 1024 * 1024
    ? (stat.size / 1024 / 1024).toFixed(1) + ' MB'
    : (stat.size / 1024).toFixed(1) + ' KB'
  return { name: file, size, url: `/files/${file}` }
})
fs.writeFileSync(path.join(FILES_DIR, 'index.json'), JSON.stringify(fileList, null, 2), 'utf-8')
console.log(`  generated: files/index.json (${fileList.length} files)`)

// ── Copy profile.json to public ──
const PROFILE_SRC = path.resolve(ROOT, 'src/profile.json')
const PROFILE_DEST = path.resolve(ROOT, 'public/profile.json')
if (fs.existsSync(PROFILE_SRC)) {
  fs.copyFileSync(PROFILE_SRC, PROFILE_DEST)
  console.log('  copied: profile.json')
}

// ── Copy albums.json to public ──
const ALBUMS_SRC = path.resolve(ROOT, 'src/albums.json')
const ALBUMS_DEST = path.resolve(ROOT, 'public/albums.json')
if (fs.existsSync(ALBUMS_SRC)) {
  fs.copyFileSync(ALBUMS_SRC, ALBUMS_DEST)
  console.log('  copied: albums.json')
}

console.log('\n✓ Static data generation complete!')
