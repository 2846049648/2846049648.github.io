import { ref, onMounted, onUnmounted } from 'vue'

export function useToc() {
  const headings = ref([])
  const activeId = ref('')

  function generateToc(htmlContent) {
    const parser = new DOMParser()
    const doc = parser.parseFromString(htmlContent, 'text/html')
    const elements = doc.body.querySelectorAll('h1, h2, h3')
    const items = []
    elements.forEach((el) => {
      const id = el.textContent
        .toLowerCase()
        .replace(/[^\w一-龥]+/g, '-')
        .replace(/(^-|-$)/g, '')
      el.id = id
      items.push({ level: el.tagName.toLowerCase(), id, text: el.textContent })
    })
    headings.value = items
    return doc.body.innerHTML
  }

  let observer = null
  function initScrollSpy() {
    const ids = headings.value.map(h => h.id).filter(Boolean)
    if (!ids.length) return
    const els = ids.map(id => document.getElementById(id)).filter(Boolean)
    if (!els.length) return
    observer = new IntersectionObserver((entries) => {
      entries.forEach(entry => {
        if (entry.isIntersecting) activeId.value = entry.target.id
      })
    }, { rootMargin: '-80px 0px -60% 0px' })
    els.forEach(el => observer.observe(el))
  }

  onUnmounted(() => observer?.disconnect())

  return { headings, activeId, generateToc, initScrollSpy }
}
