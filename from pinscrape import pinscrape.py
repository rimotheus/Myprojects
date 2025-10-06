from pinscrape import pinscrape
details = pinscrape.scraper.scrape("messi", "output", {}, 10, 15)

if details["isDownloaded"]:
    print("\nDownloading completed !!")
    print(f"\nTotal urls found: {len(details['extracted_urls'])}")
    print(f"\nTotal images downloaded (including duplicate images): {len(details['url_list'])}")
    print(details)
else:
    print("\nNothing to download !!")
scrape("messi", f(".\Desktop\karl marx"), {}, 10, 15)
# "messi" is keyword
# "output" is path to a folder where you want to save images
# {} is proxy list if you want to add one (optional)
# 10 is a number of threads you want to use for downloading those images (optional)
# 15 is the maximum number of images you want to download (optional)