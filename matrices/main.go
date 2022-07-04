package main

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"net/url"
	"os"
	"os/exec"
	"strings"
	"time"
)

func main() {
	urls := [11]string{
		"https://suitesparse-collection-website.herokuapp.com/MM/Freescale/Freescale1.tar.gz",
		"https://suitesparse-collection-website.herokuapp.com/MM/Pajek/patents.tar.gz",
		"https://suitesparse-collection-website.herokuapp.com/MM/Norris/torso1.tar.gz",
		"https://suitesparse-collection-website.herokuapp.com/MM/Kamvar/Stanford.tar.gz",
		"https://suitesparse-collection-website.herokuapp.com/MM/FEMLAB/ns3Da.tar.gz",
		"https://suitesparse-collection-website.herokuapp.com/MM/FEMLAB/poisson3Db.tar.gz",
		"https://suitesparse-collection-website.herokuapp.com/MM/FEMLAB/sme3Db.tar.gz",
		"https://suitesparse-collection-website.herokuapp.com/MM/POLYFLOW/mixtank_new.tar.gz",
		"https://suitesparse-collection-website.herokuapp.com/MM/VLSI/ss.tar.gz",
		"https://suitesparse-collection-website.herokuapp.com/MM/Freescale/FullChip.tar.gz",
		"https://suitesparse-collection-website.herokuapp.com/MM/Gleich/wb-edu.tar.gz",
	}
	fmt.Println(len(urls))
	startOfDownload := time.Now()

	for _, url := range urls {
		cmd := exec.Command("wget", url)

		var stdBuffer bytes.Buffer

		mw := io.MultiWriter(os.Stdout, &stdBuffer)

		cmd.Stderr = mw
		cmd.Stdout = mw

		err := cmd.Run()
		if err != nil {
			fmt.Printf("%v", err)
		}
		log.Println(stdBuffer.String())
	}

	elapsed := time.Since(startOfDownload)
	fmt.Printf("Downloading file took:%s\n", elapsed.String())

	for _, name := range urls {
		fileUrl, _ := url.Parse(name)

		path := fileUrl.Path
		segments := strings.Split(path, "/")
		fileName := segments[len(segments)-1]

		cmd := exec.Command("tar -xzvf", fileName)
		cmd.Stdin = os.Stdin
		cmd.Stdout = os.Stdout
		err := cmd.Run()
		if err != nil {
			log.Fatalf("%v", err)
		}
	}

	elapsedTotal := time.Since(startOfDownload)

	fmt.Printf("Total time:%s\n", elapsedTotal.String())
}
