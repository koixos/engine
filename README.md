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

- 17 kasım: vulkan shader setup başlandı

- 20 kasım: graphics pipeline imp'e başladım birkaç gün önce ve devam ediyorum. allah kahretmesin çok uzun sürüyor. ÇÜNKÜ DOĞRU DÜZGÜN ÇALIŞAMADIM. yetmezmiş gibi spec'in ilgili kısımlarını okumam gerek. hayat neden bu kadar zorsun ki...

- 21 kasım: vulkan buffer imp tamalandı. bir sonraki aşamada ekranda bir üçgen çizebilmek için gereken setup ı yapacağım yüksek iht. ama başlamadan önce code revision yapıcam çünkü proje büyüdükçe layout um, kodlama stillerim vs karıştı. 

- 22 kasım: İLK ŞEKİLLERİMİ ÇİZDİM!!! görsel:

<p align="center">
  <img
    width="1939"
    height="1233"
    alt="image"
    src="https://github.com/user-attachments/assets/39693a8b-3735-4c48-8afb-067560946fab"
  />
</p>

- 24 kasım: uniforms ve descriptors implement done. bir de ufak bir bug vardı, karem animasyon uygulamıyordu. perspective matrix fonk.umda da bug'lar vardı onu upd ettim. şimdi artık ekrandaki quad'ıma küçülme animasyonu uygulayabiliyorum... DEMO: 

<p align="center">
  <img
    width="720"
    height="406" 
    alt="demo 2"
    src="https://github.com/user-attachments/assets/5240da84-14a4-44b3-8770-2e88f955ae0e"
  />
</p>

- 25 kasım: farklı tür bir uniform olan push const.ları implement ettim. bu sayede obje üzerinde upd yapabiliyorum. demodada görüleceği üzere objeden uzaklaşırken objeyi rotate ediyorum. DEMO:

<p align="center">
  <img
    width="720"
    height="410" 
    alt="demo 3"
    src="https://github.com/user-attachments/assets/84a092c8-8cf7-413d-9d91-fefd56885131"
  />
</p>
---
**TODOs**

- should test the code on windows machine 
- should check funcs for unused params