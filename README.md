1) source ~/vulkan/1.4.363.0/setup-env.sh -- add this command to the end of ~/.profile

2) sudo apt install libvulkan-dev -- execute this to handle -lvulkan not found error

3) file permission settigns for perm-denied err when executing: chmod u+x program_name
---

- son 5 günde platform layer ımın üstüne entry point'imi, event, input, logging ve memory subsystem'larımı implement ettim.

- 1 Kasım: vulkan renderer subsystem implementiation'ına başlıyorum. bir yandan vulkan 1.4 spec'e çalışıyorum. renderer subsys imp setup ı tamamlandı, app'e entegre edildi.
- vulkan ext.s, val layers ve debugger implement edildi
- renderpass de implement ettim ancak vulkan spec e çalışmayı ertelediğim için şu anda pek bir şey anlamıyorumn. vulkan renderer component ve layer implementation ları kendi içlerinde benzer bir template a sahip ama vulkan api ın anlam ve önemi spec e çalışmadan anlaşılacak gibi değil. bu sebepten ötürü 7 kasım itibariyle implement e ara verip spec çalışacağım.

- 8 kasım: spec e çalıştıkça imp.i dahay iyi anlıyorum. vulkan'ın çalışma mantığı iyice oturmaya başladı kafamda. engine ekranımda ilk kez bir renk oluşturabildim. window resizing event'leri çalışır halde. sonraki aşama custom math lib oluşturma galiba. ufak tefek bug'lar var, fix attıktan sonra devam edeceğim.

- window minimize, shutdown, resize eventleri sorunsuz şekilde çalışıyor. DEMO: 

<p align="center">
  <img
    src="https://github.com/user-attachments/assets/784a312a-e9cc-4be3-b31d-d241ef9d3c1b"
    width="720"
    height="451"
    alt="Project Demo"
  />
</p>

- 12 kasım: custom math lib implementation ını tamamladım. bundan sonraki adım yüksek ihtimal graphical pipeline üzerine çalışmak olacak.

- 13 kasım: devam etmeden önce projede değiştirmek gereken ufak tefek detaylarla ilgilendim (TRUE/FALSE macro kullanımı, vla kullanımının kaldırılması, ufak makefile değişiklikleri).
- Ayrıca mem subsys'a lin.allocator eklendi. bu sayede ihtiyaç duyulan alan mem fragmentation veya free vb endişeleri olmadan allocate edilebilecek [reading](https://nfrechette.github.io/2015/05/21/linear_allocator/).
- Son olarak unit test geliştirmek ve gerçekleştirmek için bir test env kurdum. bu sayede oluşturduğum component'ları kendi içlerinde test edip herhangi bir bug veya error var mı tespit edebiliyorum. 
- subsystem'ların memory yönetim şeklini değiştirdim. artık her bir subsystem aynı vulkan componentları için mem ayırırken yapmamız gerektiği gibi double-call ile initialize edilmeli.

---
**TODOs**

- should test the code on windows machine 
