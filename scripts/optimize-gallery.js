/**
 * Gallery image optimizer
 *
 * Converts gallery images to WebP thumbnails to save space on GitHub.
 * Usage: node scripts/optimize-gallery.js
 *
 * - Original images are kept in public/images/gallery/{album}/
 * - WebP thumbnails are generated alongside with _thumb suffix
 * - Updates albums.json to reference WebP thumbnails
 */

import fs from 'fs'
import path from 'path'
import { fileURLToPath } from 'url'
import sharp from 'sharp'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const ROOT = path.resolve(__dirname, '..')
const GALLERY_DIR = path.resolve(ROOT, 'public/images/gallery')
const ALBUMS_PATH = path.resolve(ROOT, 'src/albums.json')

const THUMB_WIDTH = 800
const THUMB_QUALITY = 80

async function optimizeImage(filePath) {
  const ext = path.extname(filePath).toLowerCase()
  if (!['.jpg', '.jpeg', '.png', '.webp'].includes(ext)) return null

  const dir = path.dirname(filePath)
  const name = path.basename(filePath, ext)
  const thumbName = `${name}_thumb.webp`
  const thumbPath = path.join(dir, thumbName)

  // Skip if thumbnail already exists and is newer than source
  if (fs.existsSync(thumbPath)) {
    const srcStat = fs.statSync(filePath)
    const thumbStat = fs.statSync(thumbPath)
    if (thumbStat.mtimeMs > srcStat.mtimeMs) return thumbName
  }

  try {
    await sharp(filePath)
      .resize(THUMB_WIDTH, undefined, { withoutEnlargement: true })
      .webp({ quality: THUMB_QUALITY })
      .toFile(thumbPath)
    console.log(`  ✓ ${path.basename(filePath)} → ${thumbName}`)
    return thumbName
  } catch (err) {
    console.error(`  ✗ ${path.basename(filePath)}: ${err.message}`)
    return null
  }
}

async function scanAndOptimize() {
  console.log('\n🔍 Scanning gallery images...\n')

  if (!fs.existsSync(GALLERY_DIR)) {
    console.log('  No gallery directory found.')
    return
  }

  const albums = fs.readdirSync(GALLERY_DIR, { withFileTypes: true })
    .filter(d => d.isDirectory())

  for (const album of albums) {
    const albumDir = path.join(GALLERY_DIR, album.name)
    const files = fs.readdirSync(albumDir).filter(f => {
      const ext = path.extname(f).toLowerCase()
      return ['.jpg', '.jpeg', '.png', '.webp'].includes(ext) && !f.includes('_thumb.')
    })

    if (!files.length) {
      console.log(`  ${album.name}: no images found`)
      continue
    }

    console.log(`\n📁 ${album.name} (${files.length} images):`)
    const thumbs = []
    for (const file of files) {
      const thumb = await optimizeImage(path.join(albumDir, file))
      if (thumb) thumbs.push(thumb)
    }
    console.log(`  → ${thumbs.length}/${files.length} optimized`)
  }
}

async function updateAlbumsJson() {
  if (!fs.existsSync(ALBUMS_PATH)) {
    console.log('\n⚠️  src/albums.json not found, skipping index update.')
    return
  }

  console.log('\n📝 Updating albums.json to use WebP thumbs...')
  const albums = JSON.parse(fs.readFileSync(ALBUMS_PATH, 'utf-8'))
  let changed = false

  for (const album of albums) {
    if (!album.photos) continue
    album.photos = album.photos.map(photo => {
      const url = typeof photo === 'string' ? photo : photo.url
      if (!url) return photo

      // Replace image URLs with thumb versions
      const thumbUrl = url.replace(/(\.\w+)$/, '_thumb.webp')
      const thumbPath = path.join(ROOT, 'public', thumbUrl)
      if (fs.existsSync(thumbPath)) {
        changed = true
        // Convert string to object if needed
        if (typeof photo === 'string') {
          return { url: thumbUrl, original: url }
        }
        return { ...photo, url: thumbUrl, original: photo.url }
      }
      return photo
    })
  }

  if (changed) {
    fs.writeFileSync(ALBUMS_PATH, JSON.stringify(albums, null, 2), 'utf-8')
    console.log('  ✓ albums.json updated')
  } else {
    console.log('  No changes needed')
  }
}

async function main() {
  await scanAndOptimize()
  await updateAlbumsJson()
  console.log('\n✅ Gallery optimization complete!\n')
}

main().catch(console.error)
