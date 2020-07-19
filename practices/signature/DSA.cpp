//package main

// import(
//     "crypto/dsa"
//     "crypto/rand"
//     "fmt"
// )

func main() {
    var params dsa.Parameters

    //生成参数 
    if e := dsa.GenerateParameters(&params, rand.Reader, dsa.L1024N160); e != nil {
        fmt.Println(e)
    }

    //生成私钥 
    var priv dsa.PrivateKey

    priv.Parameters = params
    if e := dsa.GenerateKey(&priv, rand.Reader); e != nil {
        fmt.Println(e)
    }

    //根据私钥生成公钥 
    pub := priv.PublicKey

    //消息 
    message := []byte("hello world")

    //使用私钥进行签名，产生整数对(r,s) 
    r, s, e := dsa.Sign(rand.Reader, &priv, message)
    if e != nil {
        fmt.Println(e)
    }

    //认证 
    fmt.Printf("认证 %q (r:%s,s:%s)\n", message, r, s)
    if dsa.Verify(&pub, message, r, s) {
        fmt.Println("认证正确！")
    } else {
        fmt.Println("认证失败！")
    }
}