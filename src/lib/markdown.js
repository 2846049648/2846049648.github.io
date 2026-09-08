import { marked } from 'marked'
import { markedHighlight } from 'marked-highlight'
import hljs from 'highlight.js'

// 正文里用作视觉分隔的 `---`(紧跟在一段文字之后、中间无空行)会被 CommonMark
// 解析成 Setext 二级标题,导致整段正文被吞成 <h2> 混进目录导航。
// 全站文章均以 `#` 书写标题、没有刻意使用 Setext 语法,故在此统一禁用:
// `text\n---` 之后只会得到 <p> + <hr>,而不会再有正文被当成标题。
marked.use({
  tokenizer: {
    lheading() { return undefined },
  },
})

marked.use(markedHighlight({
  langPrefix: 'hljs language-',
  highlight(code, lang) {
    if (lang && hljs.getLanguage(lang)) {
      try { return hljs.highlight(code, { language: lang }).value } catch {}
    }
    return hljs.highlightAuto(code).value
  },
}))

export { marked }

// breaks: 单次回车即换行 —— 与编辑器实时预览保持一致
export function renderMarkdown(src) {
  return marked.parse(src, { breaks: true, gfm: true })
}
