1) source ~/vulkan/1.4.363.0/setup-env.sh -- add this command to the end of ~/.profile

2) sudo apt install libvulkan-dev -- execute this to handle -lvulkan not found error

---

- son 5 günde platform layer ımın üstüne entry point'imi, event, input, logging ve memory subsystem'larımı implement ettim.
- 1 Kasım itibariyle vulkan renderer subsystem implementiation'ına başlıyorum. bir yandan vulkan 1.4 spec'e çalışıyorum. renderer subsys imp setup ı tamamlandı, app'e entegre edildi.
- vulkan ext.s, val layers ve debugger implement edildi

---
**TODOs**

- should test the code on windows machine 
