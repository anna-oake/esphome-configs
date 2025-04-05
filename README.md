These are my esphome configs. Honestly, the only reason they are here is for me to keep track and organise them better. I am not planning to document any of this stuff.

If you find any of them useful, that's cool, but I don't promise any help/support with that.

> [!NOTE]  
> Rename `secrets.yaml.example` to `secrets.yaml` and provide your credentials. I included mine as an example, but they would only work for you if you come over to set up your devices at my place.

> [!WARNING]  
> Check `common/base.yaml`. There is `fast_connect: true` in the `wifi` component configuration. It's generally not the best thing to have enabled, especially if your SSID is broadcasted by multiple access points. However, I enabled it, because it is **required** when connecting to **hidden networks**. Remove it if yours is not hidden.