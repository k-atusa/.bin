import os
import runpod
from diffusers import DiffusionPipeline
import torch
import base64
from io import BytesIO

MODEL_ID = os.environ["MODEL_ID"]

print(f"[INFO] Loading model: {MODEL_ID}")
pipe = DiffusionPipeline.from_pretrained(
    MODEL_ID,
    torch_dtype=torch.float16
).to("cuda")
print("[INFO] Model loaded and ready.")

def generate_image(prompt, negative_prompt, steps=30, scale=7.5, seed=None, width=512, height=512):
    generator = torch.Generator("cuda").manual_seed(seed) if seed is not None else None
    image = pipe(
        prompt=prompt,
        negative_prompt=negative_prompt,
        num_inference_steps=steps,
        guidance_scale=scale,
        width=width,
        height=height,
        generator=generator
    ).images[0]
    buffered = BytesIO()
    image.save(buffered, format="PNG")
    return base64.b64encode(buffered.getvalue()).decode()

def handler(event):
    input_data = event["input"]
    prompt = input_data.get("prompt", "A beautiful landscape")
    negative_prompt = input_data.get("negative_prompt", "low quality, blurry, deformed")
    steps = int(input_data.get("steps", 30))
    scale = float(input_data.get("scale", 7.5))
    seed = input_data.get("seed", None)
    width = int(input_data.get("width", 512))
    height = int(input_data.get("height", 512))
    return {"image_base64": generate_image(prompt, negative_prompt, steps, scale, seed, width, height)}

runpod.serverless.start({"handler": handler})
