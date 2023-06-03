// Logic to move the crosshair around the screen according to the
// position recieved from the sensors.
import { TweenMax } from 'gsap';

class Crosshair {
  constructor(options) {
    this.position = options.position;
    this.visible = false;
    this.options = options;

    // Create an image element
    this.image = document.createElement('img');
    this.image.src = 'crosshair.png';
    this.image.style.position = 'absolute';
    this.image.style.width = options.imageWidth + 'px';
    this.image.style.height = options.imageHeight + 'px';

    // Append the image to the document body
    document.body.appendChild(this.image);
  }

  show() {
    this.visible = true;
    this.image.style.display = 'block';
  }

  hide() {
    this.visible = false;
    this.image.style.display = 'none';
  }

  moveTo(coords) {
    const screenWidth = window.innerWidth;
    const screenHeight = window.innerHeight;

    // Calculate the position based on percentages
    const posX = (screenWidth * coords.x);
    const posY = (screenHeight * coords.y);

    this.timeline = new TimelineMax();
    this.timeline.to(this.position, 0.5, {
      x: posX,
      y: posY,
      onUpdate: () => {
        // Update crosshair position on the screen
        this.image.style.left = this.position.x + 'px';
        this.image.style.top = this.position.y + 'px';
      },
      onComplete: () => {
        // Handle completion
      },
    });
  }
}

export default Crosshair;