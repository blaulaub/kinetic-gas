#include "frame_event.h"

#include "renderer/renderer.h"

void FrameEvent::process()
{
  renderer.render(particles);
}
