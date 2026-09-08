<script setup>
defineOptions({ name: 'TocTree' })

const props = defineProps({
  nodes: { type: Array, required: true },
  activeId: { type: String, default: '' },
})

const emit = defineEmits(['navigate'])

function onClick(e, id) {
  e.preventDefault()
  emit('navigate', id)
}
</script>

<template>
  <ul class="toc-tree">
    <li v-for="node in nodes" :key="node.id || node.text">
      <a
        :href="'#' + node.id"
        :class="['toc-link', node.level, { active: activeId === node.id }]"
        @click="onClick($event, node.id)"
      >{{ node.text }}</a>
      <TocTree
        v-if="node.children && node.children.length"
        :nodes="node.children"
        :active-id="activeId"
        @navigate="(id) => emit('navigate', id)"
      />
    </li>
  </ul>
</template>
