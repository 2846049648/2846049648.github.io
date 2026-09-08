import { ref, onMounted, onUnmounted } from 'vue'

const LEVEL = { h1: 1, h2: 2, h3: 3 }

// 把扁平的 headings 列表按文档顺序组装成树(h1 > h2 > h3):
// 低一级标题挂到最近一个高一级标题之下,实现「高级目录包含低级目录」。
export function buildTocTree(headings) {
  const root = []
  const stack = [] // { node, level }
  for (const h of headings) {
    const node = { ...h, children: [] }
    const lv = LEVEL[h.level] ?? 3
    while (stack.length && stack[stack.length - 1].level >= lv) stack.pop()
    if (stack.length) stack[stack.length - 1].node.children.push(node)
    else root.push(node)
    stack.push({ node, level: lv })
  }
  return root
}

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
